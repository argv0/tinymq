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

namespace a = boost::asio;

namespace tinymq {

connection::connection(a::io_service& io_service, server *server)
  : io_service_(io_service),
    socket_(io_service),
    server_(server),
    client_(),
    curmsg_() {
}

connection::~connection() {

}

a::ip::tcp::socket& connection::socket() {
    return socket_;
}

void connection::start() {
    std::string client_ip = socket_.remote_endpoint().address().to_string();
    std::string client_port = boost::lexical_cast<std::string>(
            socket_.remote_endpoint().port());
    //    LAPP_ << "got TCP connection from " << client_ip << ":" << client_port;
    //read_next_command();
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
        client_->enqueue_message((unsigned char *)data, incoming_.size());
        incoming_.resize(0);
        /*
        a::async_write(socket_, a::buffer((void*)&mid, sizeof(message_id)),
                 boost::bind(&connection::handle_write, shared_from_this(),
                         a::placeholders::error,
                         a::placeholders::bytes_transferred));

        */

    }
    else {
        printf("handle_read_message error %s\n", e.message().c_str());
    }
}

void connection::handle_write(const boost::system::error_code& e,
            std::size_t bytes_transferred) {
    /*
    if (!e)
        //read_next_command();
    else
        printf("handle_write error %s\n", e.message().c_str());
    */
}

} // namespace tinymq
