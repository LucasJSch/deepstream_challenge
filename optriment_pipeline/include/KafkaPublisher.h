#pragma once

#include <librdkafka/rdkafkacpp.h>

#include <cstring>
#include <iostream>
#include <memory>
#include <string>

#include "Serializable.h"

namespace optriment {

// TODO: Implement rate of publishing. With QOS and queue.
class KafkaPublisher {
   public:
    KafkaPublisher(const std::string& brokers, const std::string& topic);
    void publish(const std::shared_ptr<Serializable> message);

   private:
    std::shared_ptr<RdKafka::Conf> conf_;
    std::shared_ptr<RdKafka::Producer> producer_;
    std::string brokers_;
    std::string topic_;
};
}  // namespace optriment