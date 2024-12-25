#include <iostream>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <csignal>
#include <cstring>
#include <sstream>
#include <memory>

#if _AIX
#include <unistd.h>
#endif

#include <librdkafka/rdkafkacpp.h>


static volatile sig_atomic_t run = 1;

static void sigterm(int sig) {
  run = 0;
}

class Serializable 
{
public:
  virtual char* toBytes() const = 0;
  virtual void fromBytes(char* ptr, size_t len) = 0;
};

struct CustomMessage : public Serializable
{
    size_t zone1_count;
    size_t zone2_count;
    
    char* toBytes() const override
    {
        std::stringstream ss;
        ss << "{" << std::endl;
        ss << "\"zone1_count\": " << zone1_count << "," << std::endl;
        ss << "\"zone2_count\": " << zone2_count << std::endl;
        ss << "}" << std::endl;
        std::string str = ss.str();
        char* result = new char[str.size() + 1];
        std::strcpy(result, str.c_str());
        return result;
    }
    
    void fromBytes(char* ptr, size_t len) override
    {
        std::string str(ptr, len);
        std::stringstream ss(str);
        std::string token;
        while (std::getline(ss, token, ','))
        {
            std::string key;
            std::string value;
            std::stringstream token_ss(token);
            std::getline(token_ss, key, ':');
            std::getline(token_ss, value, ':');
            if (key == "\"zone1_count\"")
            {
                zone1_count = std::stoi(value);
            }
            else if (key == "\"zone2_count\"")
            {
                zone2_count = std::stoi(value);
            }
        }
    }
};

class ExampleDeliveryReportCb : public RdKafka::DeliveryReportCb {
 public:
  void dr_cb(RdKafka::Message &message) {
    /* If message.err() is non-zero the message delivery failed permanently
     * for the message. */
    if (message.err())
      std::cerr << "% Message delivery failed: " << message.errstr()
                << std::endl;
    else
      std::cerr << "% Message delivered to topic " << message.topic_name()
                << " [" << message.partition() << "] at offset "
                << message.offset() << std::endl;
  }
};

class CustomKafkaProducer {
 public:
  CustomKafkaProducer(const std::string& brokers, const std::string& topic)
  {
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

  void sendMessage(const std::shared_ptr<Serializable> message)
  {
    char* payload = message->toBytes();
    producer_->produce(
        topic_,
        RdKafka::Topic::PARTITION_UA,
        RdKafka::Producer::RK_MSG_COPY,
        payload, strlen(payload),
        NULL,
        0,
        0,
        NULL,
        NULL);
  }

private:
  std::shared_ptr<RdKafka::Conf> conf_;
  std::shared_ptr<RdKafka::Producer> producer_;
  std::string brokers_;
  std::string topic_;
};

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <brokers> <topic>\n";
    exit(1);
  }


  CustomKafkaProducer producer(argv[1], argv[2]);

  for (std::string line; run && std::getline(std::cin, line);) {
    if (line.empty()) {
      continue;
    }

    CustomMessage message;
    message.zone1_count = 10;
    message.zone2_count = 20;
    producer.sendMessage(std::make_shared<CustomMessage>(message));
  }
  std::cerr << "% Flushing final messages..." << std::endl;

  return 0;
}
