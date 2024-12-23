import json
from confluent_kafka import Consumer, KafkaException, KafkaError

# Kafka configuration based on DeepStream msgbroker settings
conf = {
    'bootstrap.servers': 'localhost:9092',  # Kafka broker connection string
    'group.id': 'deepstream-consumer-group',  # Consumer group ID (can be customized)
    'auto.offset.reset': 'earliest'  # Start from the earliest message
}

# Create a Kafka consumer instance
consumer = Consumer(conf)

# Kafka topic from msgbroker config
topic = 'topic1'

# Subscribe to the Kafka topic
consumer.subscribe([topic])

def consume_messages():
    try:
        while True:
            # Poll for new messages (blocking call)
            msg = consumer.poll(timeout=1.0)  # Adjust timeout based on your needs

            if msg is None:  # No message, timeout
                continue
            if msg.error():
                if msg.error().code() == KafkaError._PARTITION_EOF:
                    # End of partition, no more messages
                    print(f"End of partition reached {msg.partition} @ {msg.offset}")
                else:
                    # Error while consuming
                    raise KafkaException(msg.error())
            else:
                # Successfully received message
                message_value = msg.value().decode('utf-8')
                try:
                    # Assuming the payload is JSON (as suggested by payload-type: 0)
                    message_data = json.loads(message_value)
                    print(f"Consumed message: {json.dumps(message_data, indent=4)}")

                except json.JSONDecodeError as e:
                    # Handle error if the message is not valid JSON
                    print(f"Error decoding message: {e}")
                    print(f"Message value: {message_value}")

    except KeyboardInterrupt:
        print("Consumer interrupted, closing...")

    finally:
        # Gracefully close the consumer
        consumer.close()

if __name__ == "__main__":
    consume_messages()
