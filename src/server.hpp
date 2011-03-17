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

#ifndef TINYMQ_SERVER_HPP__
#define TINYMQ_SERVER_HPP__

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include "connection.hpp"
#include "message_queue.hpp"

namespace tinymq {

class server : private boost::noncopyable {
public:
  explicit server(const std::string& address, const std::string& port,
      const std::string& doc_root);
  void run();
  void stop();
  message_queue& queue()  { return message_queue_; }
private:
  void handle_accept(const boost::system::error_code& e);
  boost::asio::io_service io_service_;
  boost::asio::ip::tcp::acceptor acceptor_;
  connection_ptr new_connection_;
  message_queue message_queue_;
};

} // namespace tinymq

#endif // include guard
