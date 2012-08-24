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

#ifndef TINYMQ_CONNECTION_HPP_
#define TINYMQ_CONNECTION_HPP_

#include <vector>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/asio/local/connect_pair.hpp>
#include "client.hpp"

namespace tinymq {

class server;

class connection : public boost::enable_shared_from_this<connection>,
                   private boost::noncopyable {
    boost::asio::io_service& io_service_;
    boost::asio::ip::tcp::socket socket_;
    boost::array<char, 8192> buffer_;
    std::vector<char> incoming_;
    server *server_;
    client::shared_ptr client_;
    message_shared_ptr curmsg_;
public:
    enum command { // command ids
        get_msg = 0,
        put_msg = 2
    };
    enum { command_length = 8 };
public: // construction
    explicit connection(boost::asio::io_service& io_service, server *server);
    virtual ~connection();
public: // api
    boost::asio::ip::tcp::socket& socket();
    void start();
private:
     void handle_read_length(const boost::system::error_code& e,
        std::size_t bytes_transferred);
     void handle_read_message(const boost::system::error_code& e,
        std::size_t bytes_transferred);
    void handle_write(const boost::system::error_code& e,
        std::size_t bytes_transferred);
};

typedef boost::shared_ptr<connection> connection_ptr;

} // namespace tinymq

#endif // include guard
