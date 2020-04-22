#!/usr/bin/env python
import pika

HOST = "127.0.0.1"
PORT = 5672
CHANNEL_NO = 1
QUEUE_NAME = "TEST_QUEUE"


def message_callback(channel, method, properties, body: bytes):
    print(body.decode('ascii'))


if __name__ == '__main__':
    connection = pika.BlockingConnection(pika.ConnectionParameters(HOST, PORT))
    channel = connection.channel(CHANNEL_NO)
    queue = channel.queue_declare(queue=QUEUE_NAME)
    channel.queue_bind(queue=QUEUE_NAME,
                       exchange="amq.direct",
                       routing_key=QUEUE_NAME)

    channel.basic_consume(queue=QUEUE_NAME,
                          auto_ack=False,
                          on_message_callback=message_callback)

    channel.start_consuming()

    connection.close()
