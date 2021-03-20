# 基本概念：

Kafka中发布订阅的对象是topic。我们可以为每类数据创建一个topic。

把向topic发布消息的客户端称作producer，从topic订阅消息的客户端称作consumer。

producers和consumers可以同时从多个topic读写数据。

一个kafka集群由一个或多个broker服务器组成，它负责持久化和备份具体的kafka消息。

![架构](.\res\pic\架构.jpg)

- topic：消息存放的目录即主题
- producer：生产消息到topic的一方
- consumer：订阅topic消费消息的一方
- broker：Kafka的服务实例就是一个broker



ZooKeeper用于分布式系统的协调，Kafka使用ZooKeeper也是基于相同的原因。

ZooKeeper主要用来协调Kafka的各个broker，不仅可以实现broker的负载均衡，而且当增加了broker或者某个broker故障了，ZooKeeper将会通知生产者和消费者，这样可以保证整个系统正常运转。



# 服务端：

服务端可以简单理解为基本概念架构图中的`Kafka Broker`。多个`Broker`构成`Cluster`。

下载官方编译好的安装包[kafka_2.11-1.0.0.tgz](http://kafka.apache.org/downloads)，具体安装步骤参考[kafka安装及快速入门](http://www.54tianzhisheng.cn/2018/01/04/Kafka/)。在`linux`上安装kafka服务端程序。



启动`zookeeper`：

```shell
bin/zookeeper-server-start.sh -daemon config/zookeeper.properties
```

启动`kafka`服务：

```shell
bin/kafka-server-start.sh -daemon config/server.properties
```

建立名字为test的`topic`：

```bash
bin/kafka-topics.sh --create --zookeeper localhost:2181 --replication-factor 1 --partitions 1 --topic test
```

查看已有的`topic`：

```bash
bin/kafka-topics.sh --list --zookeeper localhost:2181
```



运行下官方提供的示例，生产者发送消息：

```shell
bin/kafka-console-producer.sh --broker-list localhost:9092 --topic test
```

![生产者](.\res\pic\producer.png)



重新打开一个终端，消费者收到生产者发送的消息：

```shell
bin/kafka-console-consumer.sh --zookeeper localhost:2181 --topic test --from-beginning
```

![消费者](.\res\pic\consumer.png)



# 客户端：

客户端可以简单理解为基本概念架构图中的`producer`和`consumer`。

这里主要介绍官方提供的[kafka C++客户端](https://github.com/edenhill/librdkafka)：

1. rdkafka : kafka提供的c接口。

2. rdkafkacpp: kafka提供的c++接口。

在实际使用中发现，window下librdkafka 需要使用到`zlib`库。如果缺少的话，程序运行时会出现：`0xC000041D: 用户回调期间遇到未经处理的异常`

服务端要配置服务器监听端口，在`server.properties`中修改：

```shell
# The address the socket server listens on. It will get the value returned from 
# java.net.InetAddress.getCanonicalHostName() if not configured.
#   FORMAT:
#     listeners = listener_name://host_name:port
#   EXAMPLE:
#     listeners = PLAINTEXT://your.host.name:9092
listeners=PLAINTEXT://:9092

# Hostname and port the broker will advertise to producers and consumers. If not set, 
# it uses the value for "listeners" if configured.  Otherwise, it will use the value
# returned from java.net.InetAddress.getCanonicalHostName().
advertised.listeners=PLAINTEXT://10.67.76.9:9092
```



## 消费者：

在这个例子里，kafka服务端充当生产者，运行官方脚本：

```shell
bin/kafka-console-producer.sh --broker-list localhost:9092 --topic test
```

在命令行里发送消息。我们编写的客户端作为消费者，从kafka读取消息。效果如下：
![result](.\res\pic\result.png)

客户端消费者代码如下：

```c++
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
```


## 生产者：

在这个例子里，我们编写生产者程序，将消息发送到kafka。

上面编写的消费者程序，从kafka读取生产者发送的消息。



```cpp
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

```





# 参考文献：

1. [使用librdkafka的C++接口实现简单的生产者和消费者](https://blog.csdn.net/caoshangpa/article/details/79786100)
3. [c++（11）使用librdkafka库实现kafka的消费实例](https://blog.csdn.net/lijinqi1987/article/details/76691170/)
3. [kafka安装及快速入门](http://www.54tianzhisheng.cn/2018/01/04/Kafka/)
4. [kafka端口号修改](https://blog.csdn.net/lizz861109/article/details/109093852)
5. [apach kafka quickstart](http://kafka.apache.org/082/documentation.html#quickstart)
6. [kafka C++客户端使用](https://www.jianshu.com/p/6aaec5bf00c2)







