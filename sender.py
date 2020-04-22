#!/usr/bin/env python
import pika
import sys

HOST = "127.0.0.1"
PORT = 5672
CHANNEL_NO = 1
QUEUE_NAME = "TEST_QUEUE"

if __name__ == '__main__':
    connection = pika.BlockingConnection(pika.ConnectionParameters(HOST, PORT))
    channel = connection.channel(CHANNEL_NO)
    queue = channel.queue_declare(queue=QUEUE_NAME)

    while True:
        line = sys.stdin.readline()
        if line is None:
            break
        line = line[:-1]
        channel.basic_publish(exchange="amq.direct",
                              routing_key=QUEUE_NAME,
                              body=line.encode('ascii'))

    connection.close()
