//
// Created by wcl on 2020/04/22.
//

#pragma once
extern "C" {
#include <amqp.h>
#include <amqp_tcp_socket.h>
}

#include <string>
#include <functional>

amqp_connection_state_t open_rabbit();

amqp_bytes_t declare_queue(const amqp_connection_state_t &con);

bool send_message(const amqp_connection_state_t &con, const std::string &str);

bool receive_message(const amqp_connection_state_t &con, const amqp_bytes_t &queue_name,
                     const std::function<void(const std::string &)> &func);

bool close_rabbit(const amqp_connection_state_t &connection);
