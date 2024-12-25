#include "KafkaPublisher.h"

using namespace optriment;

KafkaPublisher::KafkaPublisher(const std::string& brokers, const std::string& topic) {
    conf_ = std::shared_ptr<RdKafka::Conf>(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));
    brokers_ = brokers;
    topic_ = topic;

    std::string errstr;
    if (conf_->set("bootstrap.servers", brokers, errstr) != RdKafka::Conf::CONF_OK) {
        std::cerr << errstr << std::endl;
        exit(1);
    }

    producer_ = std::shared_ptr<RdKafka::Producer>(RdKafka::Producer::create(conf_.get(), errstr));
    if (!producer_) {
        std::cerr << "Failed to create producer: " << errstr << std::endl;
        exit(1);
    }
}

void KafkaPublisher::publish(const std::shared_ptr<Serializable> message) {
    char* payload = message->toBytes();
    producer_->produce(topic_, RdKafka::Topic::PARTITION_UA, RdKafka::Producer::RK_MSG_COPY, payload, strlen(payload),
                       NULL, 0, 0, NULL, NULL);
}