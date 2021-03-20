#include <iostream>
#include <string>
#include <list>
#include <stdint.h>
#include "rdkafka.h"
#include "rdkafkacpp.h"
#include <list>
using namespace std;

void dump_config(RdKafka::Conf* conf) 
{
	std::list<std::string> *dump = conf->dump();
	printf("config dump(%d):\n", (int32_t)dump->size());
	for (auto it = dump->begin(); it != dump->end();) 
	{
		std::string name = *it++;
		std::string value = *it++;
		printf("%s = %s\n", name.c_str(), value.c_str());
	}
	printf("---------------------------------------------\n");
}

void msg_consume(RdKafka::Message* msg)
{
	if (msg->err() == RdKafka::ERR_NO_ERROR)
	{
		std::cout << "Read msg at offset " << msg->offset() << std::endl;
		if (msg->key())
		{
			std::cout << "Key: " << *msg->key() << std::endl;
		}
		printf("%.*s\n", static_cast<int>(msg->len()), static_cast<const char *>(msg->payload()));
	}
	else if (msg->err() == RdKafka::ERR__TIMED_OUT)
	{
		// 读完了 再读取 状态码为RdKafka::ERR__TIMED_OUT
		printf("error[%s]\n", "ERR__TIMED_OUT");
	}
	else
	{
		printf("error[%s]\n", "other");
	}
}

int main(int argc, char **argv)
{
	string err_string;
	int32_t partition = RdKafka::Topic::PARTITION_UA;
	partition = 0;

	std::string broker_list = "10.67.76.9:9092";

	RdKafka::Conf* global_conf = RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL);
	RdKafka::Conf* topic_conf = RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC);

	int64_t start_offset = RdKafka::Topic::OFFSET_BEGINNING;
	global_conf->set("metadata.broker.list", broker_list, err_string);

	//dump_config(global_conf);
	//dump_config(topic_conf);

	// create consumer
	RdKafka::Consumer* consumer = RdKafka::Consumer::create(global_conf, err_string);
	if (!consumer) {
		printf("failed to create consumer, %s\n", err_string.c_str());
		return -1;
	}
	printf("created consumer %s\n", consumer->name().c_str());

	// create topic
	std::string topic_name = "test";
	RdKafka::Topic* topic = RdKafka::Topic::create(consumer, topic_name, topic_conf, err_string);
	if (!topic) {
		printf("try create topic[%s] failed, %s\n", topic_name.c_str(), err_string.c_str());
		return -1;
	}

	// Start consumer for topic+partition at start offset
	RdKafka::ErrorCode resp = consumer->start(topic, partition, start_offset);
	if (resp != RdKafka::ERR_NO_ERROR) {
		printf("Failed to start consumer: %s\n",RdKafka::err2str(resp).c_str());
		return -1;
	}

	while (true) 
	{
		RdKafka::Message *msg = consumer->consume(topic, partition, 2000);
		msg_consume(msg);
		delete msg;
	}

	// stop consumer
	consumer->stop(topic, partition);
	consumer->poll(1000);

	delete topic;
	delete consumer;

	system("pause");
	return 0;
}