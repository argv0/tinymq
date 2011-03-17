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
#include <boost/shared_ptr.hpp>
#include "server.hpp"
#include "tinymq_log.hpp"

namespace tinymq {

server::server(const std::string& address, const std::string& port,
        const std::string& doc_root) :
    acceptor_(io_service_),
    new_connection_(new connection(io_service_, this)),
    message_queue_("tinymq.data", 52428800L) {
    LAPP_ << "queue initialized, now accepting connections on port " << port;
    boost::asio::ip::tcp::resolver resolver(io_service_);
    boost::asio::ip::tcp::resolver::query query(address, port);
    boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();
    acceptor_.async_accept(new_connection_->socket(),
            boost::bind(
                    &server::handle_accept,
                    this,
                    boost::asio::placeholders::error));
}

void server::run() {
     io_service_.run();
}

void server::stop() {
    io_service_.stop();
}

void server::handle_accept(const boost::system::error_code& e) {
    if (!e) {
        new_connection_->start();
        new_connection_.reset(new connection(io_service_, this));
        acceptor_.async_accept(new_connection_->socket(), boost::bind(
                &server::handle_accept, this, boost::asio::placeholders::error));
    }
}

} // namespace tinymq
