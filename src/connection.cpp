/*
 Copyright (c) 2009 Andy Gross <andy@andygross.org>

 This file is provided to you under the Apache License,
 Version 2.0 (the "License"); you may not use this file
 except in compliance with the License.  You may obtain
 a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing,
 software distributed under the License is distributed on an
 "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND, either express or implied.  See the License for the
 specific language governing permissions and limitations
 under the License.

 Based on example code from the boost::asio package which is:

 Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)

 Distributed under the Boost Software License, Version 1.0. (See accompanying
 file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
*/

#include <vector>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/asio/buffer.hpp>
#include "connection.hpp"
#include "server.hpp"
#include "tinymq_log.hpp"

namespace a = boost::asio;

namespace tinymq {

connection::connection(a::io_service& io_service, server *server)
  : io_service_(io_service),
    socket_(io_service),
    server_(server),
    client_(new tcp_client(&server->queue(), io_service)),
    curmsg_() {
}

connection::~connection() {

}

a::ip::tcp::socket& connection::socket() {
    return socket_;
}

void connection::read_next_command() {
    a::async_read(socket_, a::buffer(buffer_, command_length),
        boost::bind(&connection::handle_read_command, shared_from_this(),
          a::placeholders::error,
          a::placeholders::bytes_transferred));
}

void connection::wait() {
    client_->wait();
    a::async_read(client_->waker_sock(), a::buffer(buffer_, command_length),
        boost::bind(&connection::handle_resume, shared_from_this(),
                a::placeholders::error,
                a::placeholders::bytes_transferred));
}

void connection::start() {
    std::string client_ip = socket_.remote_endpoint().address().to_string();
    std::string client_port = boost::lexical_cast<std::string>(
            socket_.remote_endpoint().port());
    LAPP_ << "got TCP connection from " << client_ip << ":" << client_port;
    read_next_command();
}

void connection::cmd_get() {
    LAPP_ << "dequeueing message, qsize=" << client_->qsize();
    curmsg_ = client_->dequeue_message();
    if (!curmsg_) wait();
    else {
        const message_header& hdr(curmsg_->header());
        boost::array<a::const_buffer, 3> bufs = {{
            a::buffer((void*)&hdr.id, sizeof(message_id)),
            a::buffer((void*)&hdr.len, sizeof(message_len_t)),
            a::buffer((void*)&curmsg_->data()->front(), curmsg_->size())
            }};
        a::async_write(socket_, bufs,
                boost::bind(&connection::handle_write, shared_from_this(),
                        a::placeholders::error,
                        a::placeholders::bytes_transferred));
    }
}

void connection::cmd_ack(message_id id) {
    client_->ack_message(id);
    read_next_command();
}

void connection::handle_read_command(const boost::system::error_code& e,
    std::size_t bytes_transferred) {
    if (!e) {
        a::const_buffer b = a::buffer(buffer_);
        const command *command_type = a::buffer_cast<const command *>(b);
        if (*command_type == get_msg)  {
            cmd_get();
        }
        else if (*command_type == ack_msg) {
            a::async_read(socket_, a::buffer(buffer_, sizeof(message_id)),
                 bind(&connection::handle_read_message_id, shared_from_this(),
                   a::placeholders::error,
                   a::placeholders::bytes_transferred));
        }
        else if (*command_type == put_msg) {
            a::async_read(socket_, a::buffer(buffer_, sizeof(message_len_t)),
                 boost::bind(&connection::handle_read_length, shared_from_this(),
                   a::placeholders::error,
                   a::placeholders::bytes_transferred));
        }
        else {
            printf("ERROR: unknown message type %d, closing connection\n", *command_type);
            socket_.shutdown(
                    a::ip::tcp::socket::shutdown_both);
        }
    }
}

void connection::handle_resume(const boost::system::error_code& e,
    std::size_t bytes_transferred) {
    if (!e)
        cmd_get();
}

void connection::handle_read_message_id(const boost::system::error_code& e,
        std::size_t bytes_transferred) {
    if (!e) {
        a::const_buffer b = a::buffer(buffer_);
        const message_id *id = a::buffer_cast<const message_id*>(b);
        cmd_ack(*id);
    }
    else {
        printf("handle_read_message_id error %s\n", e.message().c_str());
    }
}

void connection::handle_read_length(const boost::system::error_code& e,
        std::size_t bytes_transferred) {
    if (!e) {
        a::const_buffer b = a::buffer(buffer_);
        const message_len_t *msg_size = a::buffer_cast<const message_len_t*>(b);
        incoming_.reserve(*msg_size);
        incoming_.insert(incoming_.begin(), *msg_size, 0);
        a::async_read(socket_, a::buffer(incoming_),
             boost::bind(&connection::handle_read_message, shared_from_this(),
               a::placeholders::error,
               a::placeholders::bytes_transferred));

    }
    else {
        printf("handle_read_length error %s\n", e.message().c_str());
    }
}

void connection::handle_read_message(const boost::system::error_code& e,
    std::size_t bytes_transferred) {
    if (!e) {
        a::const_buffer b = a::buffer(incoming_);
        const char *data = a::buffer_cast<const char*>(b);
        message_id mid = client_->enqueue_message((unsigned char *)data, incoming_.size());
        LAPP_ << "received message id " << mid << " (" << incoming_.size() << " bytes)";
        incoming_.resize(0);
        a::async_write(socket_, a::buffer((void*)&mid, sizeof(message_id)),
                 boost::bind(&connection::handle_write, shared_from_this(),
                         a::placeholders::error,
                         a::placeholders::bytes_transferred));

    }
    else {
        printf("handle_read_message error %s\n", e.message().c_str());
    }
}

void connection::handle_write(const boost::system::error_code& e,
            std::size_t bytes_transferred) {
    if (!e)
        read_next_command();
    else
        printf("handle_write error %s\n", e.message().c_str());
}

} // namespace tinymq
