import json
from confluent_kafka import Consumer, KafkaException, KafkaError

# Kafka configuration based on DeepStream msgbroker settings
conf = {
    'bootstrap.servers': 'localhost:9092',  # Kafka broker connection string
    'group.id': 'deepstream-consumer-group',  # Consumer group ID (can be customized)
    'auto.offset.reset': 'earliest'  # Start from the earliest message
}


consumer = Consumer(conf)


topic = 'topic1'


consumer.subscribe([topic])

def consume_messages():
    try:
        while True:
            msg = consumer.poll(timeout=1.0) 

            if msg is None: 
                continue
            if msg.error():
                if msg.error().code() == KafkaError._PARTITION_EOF:
                    # End of partition, no more messages
                    print(f"End of partition reached {msg.partition} @ {msg.offset}")
                else:
                    raise KafkaException(msg.error())
            else:
                message_value = msg.value().decode('utf-8')
                try:
                    message_data = json.loads(message_value)
                    print(f"Consumed message: {json.dumps(message_data, indent=4)}")

                except json.JSONDecodeError as e:
                    # Handle error if the message is not valid JSON
                    print(f"Error decoding message: {e}")
                    print(f"Message value: {message_value}")

    except KeyboardInterrupt:
        print("Consumer interrupted, closing...")

    finally:
        consumer.close()

if __name__ == "__main__":
    consume_messages()
