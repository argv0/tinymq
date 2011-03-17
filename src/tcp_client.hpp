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
*/

#ifndef TCP_CLIENT_HPP_
#define TCP_CLIENT_HPP_

#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/local/stream_protocol.hpp>
#include <boost/asio/local/connect_pair.hpp>
#include "client.hpp"

namespace tinymq {

class tcp_client : public virtual client {
public: // typedefs
    typedef boost::asio::local::stream_protocol::socket local_sock;
private:
    boost::asio::io_service& io_service_;
    local_sock notifier_;
    local_sock waker_;
public: // construction/destruction
    tcp_client(message_queue *mq, boost::asio::io_service& io_service);
    virtual ~tcp_client();
public:  // api
    local_sock& waker_sock();
    local_sock& notifier_sock();
    virtual void notify();
};

typedef boost::shared_ptr<tcp_client> tcp_client_ptr;
typedef boost::weak_ptr<tcp_client> tcp_client_wref;

}

#endif /* TCP_CLIENT_HPP_ */
