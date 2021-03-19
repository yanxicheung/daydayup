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

下载官方编译好的安装包[kafka_2.11-1.0.0.tgz](http://kafka.apache.org/downloads)，具体安装步骤参考[kafka安装及快速入门](http://www.54tianzhisheng.cn/2018/01/04/Kafka/)。在linux上安装kafka服务端程序。



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

![生产者](.\res\pic\producer.png)

重新打开一个终端，消费者收到生产者发送的消息：

![消费者](.\res\pic\consumer.png)



# 客户端：

客户端可以简单理解为基本概念架构图中的`producer`和`consumer`。

这里主要介绍官方提供的kafka C++客户端：

1. rdkafka : kafka提供的c接口。

2. rdkafkacpp: kafka提供的c++接口。

代码工程路径如下：[c++  kafka demo](.\res\code)



# 参考：

1. [使用librdkafka的C++接口实现简单的生产者和消费者](https://blog.csdn.net/caoshangpa/article/details/79786100)
3. [c++（11）使用librdkafka库实现kafka的消费实例](https://blog.csdn.net/lijinqi1987/article/details/76691170/)
4. [kafka0.8.2集群的环境搭建并实现基本的生产消费](https://blog.csdn.net/lijinqi1987/article/details/75126081)
4. [kafka安装及快速入门](http://www.54tianzhisheng.cn/2018/01/04/Kafka/)
5. [kafka端口号修改](https://blog.csdn.net/lizz861109/article/details/109093852)
6. [apach kafka quickstart](http://kafka.apache.org/082/documentation.html#quickstart)







