//
// Created by wcl on 2020/04/22.
//


#include "def.h"
#include "rabbit_mq.h"
#include <string>

amqp_connection_state_t open_rabbit() {
    amqp_connection_state_t con = amqp_new_connection();
    amqp_socket_t *socket = amqp_tcp_socket_new(con);

    int status = amqp_socket_open(socket, HOST, PORT);

    amqp_login(con, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN,
               "guest", "guest");

    amqp_channel_open(con, CHANNEL_NO);
    return con;
}

amqp_bytes_t declare_queue(const amqp_connection_state_t &con) {
    amqp_queue_declare_ok_t *r = amqp_queue_declare(con, CHANNEL_NO,
                                                    amqp_empty_bytes, 0, 0, 0, 1, amqp_empty_table);


    amqp_get_rpc_reply(con);
    auto queue_name = amqp_bytes_malloc_dup(r->queue);

    if (queue_name.bytes == NULL) {
        fprintf(stderr, "Out of memory while copying queue name");
        return queue_name;
    }

    return queue_name;
}


bool send_message(const amqp_connection_state_t &con, const std::string &str) {
    amqp_bytes_t message_bytes;
    message_bytes.len = str.size();
    message_bytes.bytes = (void *) str.c_str();
    auto status = amqp_basic_publish(con, CHANNEL_NO, amqp_cstring_bytes("amq.direct"),
                                     amqp_cstring_bytes(QUEUE_NAME), 0, 0, nullptr, message_bytes);

    return status >= 0;
}

bool receive_message(const amqp_connection_state_t &con,
                     const amqp_bytes_t &queue_name,
                     const std::function<void(const std::string &)> &func) {
    amqp_queue_bind(con, CHANNEL_NO, queue_name, amqp_cstring_bytes("amq.direct"),
                    amqp_cstring_bytes(QUEUE_NAME), amqp_empty_table);
    amqp_get_rpc_reply(con);

    amqp_basic_consume(con, CHANNEL_NO, queue_name, amqp_empty_bytes, 0, 1, 0,
                       amqp_empty_table);

    amqp_get_rpc_reply(con);

    amqp_frame_t frame;
    for (;;) {
        amqp_rpc_reply_t ret;
        amqp_envelope_t envelope;

        amqp_maybe_release_buffers(con);
        ret = amqp_consume_message(con, &envelope, nullptr, 0);

        if (AMQP_RESPONSE_NORMAL != ret.reply_type) {
            if (AMQP_RESPONSE_LIBRARY_EXCEPTION == ret.reply_type &&
                AMQP_STATUS_UNEXPECTED_STATE == ret.library_error) {
                if (AMQP_STATUS_OK != amqp_simple_wait_frame(con, &frame)) {
                    return false;
                }

                if (AMQP_FRAME_METHOD == frame.frame_type) {
                    switch (frame.payload.method.id) {
                        case AMQP_BASIC_ACK_METHOD:
                            /* if we've turned publisher confirms on, and we've published a
                             * message here is a message being confirmed.
                             */
                            break;
                        case AMQP_BASIC_RETURN_METHOD:
                            /* if a published message couldn't be routed and the mandatory
                             * flag was set this is what would be returned. The message then
                             * needs to be read.
                             */
                        {
                            amqp_message_t message;
                            ret = amqp_read_message(con, frame.channel, &message, 0);
                            if (AMQP_RESPONSE_NORMAL != ret.reply_type) {
                                return false;
                            }

                            amqp_destroy_message(&message);
                        }

                            break;

                        case AMQP_CHANNEL_CLOSE_METHOD:
                            /* a channel.close method happens when a channel exception occurs,
                             * this can happen by publishing to an exchange that doesn't exist
                             * for example.
                             *
                             * In this case you would need to open another channel redeclare
                             * any queues that were declared auto-delete, and restart any
                             * consumers that were attached to the previous channel.
                             */
                            return false;

                        case AMQP_CONNECTION_CLOSE_METHOD:
                            /* a connection.close method happens when a connection exception
                             * occurs, this can happen by trying to use a channel that isn't
                             * open for example.
                             *
                             * In this case the whole connection must be restarted.
                             */
                            return false;

                        default:
                            fprintf(stderr, "An unexpected method was received %u\n",
                                    frame.payload.method.id);
                            return false;
                    }
                }
            }

        } else {
            auto body = envelope.message.body;
            std::string msg((char *) body.bytes, body.len);
            func(msg);
            amqp_destroy_envelope(&envelope);
        }


    }

    return true;
}

bool close_rabbit(const amqp_connection_state_t &connection) {
    bool status = true;
    amqp_channel_close(connection, CHANNEL_NO, AMQP_REPLY_SUCCESS);
    amqp_connection_close(connection, AMQP_REPLY_SUCCESS);
    return amqp_destroy_connection(connection) >= 0;
}
