#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <memory>

#include "Serializable.h"

#include <librdkafka/rdkafkacpp.h>

namespace optriment {

// TODO: Separate to several files per class, and source files also.
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
} // namespace optriment