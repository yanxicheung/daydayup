#include <iostream>
#include <string>
#include <list>
#include <stdint.h>
#include "rdkafka.h"
#include "rdkafkacpp.h"
#include <list>
using namespace std;


class ExampleDeliveryReportCb : public RdKafka::DeliveryReportCb {
public:
	void dr_cb(RdKafka::Message &message) {
		/* If message.err() is non-zero the message delivery failed permanently
		* for the message. */
		if (message.err())
			std::cerr << "% Message delivery failed: " << message.errstr() << std::endl;
		else
			std::cerr << "% Message delivered to topic " << message.topic_name() <<
			" [" << message.partition() << "] at offset " <<
			message.offset() << std::endl;
	}
};

int main(int argc, char **argv) {

	std::string brokers = "10.67.76.9:9092";
	std::string topic = "test";

	RdKafka::Conf *conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);

	std::string errstr;
	/* Set bootstrap broker(s) as a comma-separated list of
	* host or host:port (default port 9092).
	* librdkafka will use the bootstrap brokers to acquire the full
	* set of brokers from the cluster. */
	if (conf->set("bootstrap.servers", brokers, errstr) !=
		RdKafka::Conf::CONF_OK) {
		std::cerr << errstr << std::endl;
		exit(1);
	}

	ExampleDeliveryReportCb ex_dr_cb;

	if (conf->set("dr_cb", &ex_dr_cb, errstr) != RdKafka::Conf::CONF_OK) {
		std::cerr << errstr << std::endl;
		exit(1);
	}

	/*
	* Create producer instance.
	*/
	RdKafka::Producer *producer = RdKafka::Producer::create(conf, errstr);
	if (!producer) {
		std::cerr << "Failed to create producer: " << errstr << std::endl;
		exit(1);
	}
	delete conf;

	/*
	* Read messages from stdin and produce to broker.
	*/
	std::cout << "% Type message value and hit enter " <<
		"to produce message." << std::endl;

	for (std::string line; true && std::getline(std::cin, line);) 
	{
		if (line.empty()) {
			producer->poll(0);
			continue;
		}

	retry:
		RdKafka::ErrorCode err = 
			producer->produce(topic, RdKafka::Topic::PARTITION_UA,
			RdKafka::Producer::RK_MSG_COPY /* Copy payload */,
			/* Value */
			const_cast<char *>(line.c_str()), line.size(),
			/* Key */
			NULL, 0,
			/* Timestamp (defaults to current time) */
			0,
			/* Message headers, if any */
			NULL);

		if (err != RdKafka::ERR_NO_ERROR) {
			std::cerr << "% Failed to produce to topic " << topic << ": " <<
				RdKafka::err2str(err) << std::endl;

			if (err == RdKafka::ERR__QUEUE_FULL) {
				producer->poll(1000/*block for max 1000ms*/);
				goto retry;
			}

		}
		else {
			std::cerr << "% Enqueued message (" << line.size() << " bytes) " <<
				"for topic " << topic << std::endl;
		}

		producer->poll(0);
	}

	std::cerr << "% Flushing final messages..." << std::endl;
	producer->flush(10 * 1000 /* wait for max 10 seconds */);

	if (producer->outq_len() > 0)
		std::cerr << "% " << producer->outq_len() <<
		" message(s) were not delivered" << std::endl;

	delete producer;

	return 0;
}
