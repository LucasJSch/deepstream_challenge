import json
from confluent_kafka import Consumer, KafkaException, KafkaError
from time import sleep

conf = {
    'bootstrap.servers': 'localhost:9092',  # Kafka broker connection string
    'group.id': 'deepstream-consumer-group',  # Consumer group ID (can be customized)
    'auto.offset.reset': 'earliest'  # Start from the earliest message
}

topic = 'topic1'
consumer = Consumer(conf)

def wait_for_topic(topic_name):
    print(f"Waiting for topic '{topic_name}' to be available...")
    while True:
        try:
            # Try subscribing to the topic
            consumer.subscribe([topic_name])

            # Try polling a message (without blocking too long)
            msg = consumer.poll(timeout=1.0)
            if msg is not None:
                if msg.error():
                    if msg.error().code() == KafkaError.UNKNOWN_TOPIC_OR_PART:
                        print(f"Topic '{topic_name}' does not exist, retrying...")
                        sleep(2)  # Wait for 2 seconds before retrying
                    else:
                        raise KafkaException(msg.error())
                else:
                    print(f"Topic '{topic_name}' exists, starting to consume messages.")
                    break
            else:
                sleep(2)
                print(f"Retrying...")

        except KafkaException as e:
            print(f"Error: {e}")
            sleep(2)

# Subscribe to the Kafka topic and consume messages
def consume_messages():
    # Wait for the topic to exist
    wait_for_topic(topic)

    try:
        while True:
            # Poll for new messages (blocking call)
            msg = consumer.poll(timeout=1.0)

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
