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

#include "tcp_client.hpp"

namespace tinymq {

tcp_client::tcp_client(message_queue *mq, boost::asio::io_service& io_service) :
    client(mq),
    io_service_(io_service),
    notifier_(io_service),
    waker_(io_service ) {
    boost::asio::local::connect_pair(notifier_, waker_);
}

tcp_client::~tcp_client() {
    notifier_.close();
    waker_.close();
}

tcp_client::local_sock& tcp_client::waker_sock() {
    return waker_;
}
tcp_client::local_sock& tcp_client::notifier_sock() {
    return notifier_;
}

void tcp_client::notify() {
    boost::asio::write(notifier_, boost::asio::buffer("0000"));
}

} // namespace tinymq
