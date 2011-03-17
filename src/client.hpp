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

#ifndef CLIENT_HPP_
#define CLIENT_HPP_

#include <map>
#include <ctime>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "message.hpp"

namespace tinymq {

class message_queue;

class client : public boost::enable_shared_from_this<client>,
               private boost::noncopyable {
public:
    typedef boost::shared_ptr<client> shared_ptr;
    typedef boost::weak_ptr<client> weak_ptr;
public:
    typedef std::pair<std::time_t, message_shared_ptr> message_receipt;
    typedef std::map<message_id, message_receipt> receipt_map;
private:
    receipt_map outstanding_;
    message_queue *queue_;
public:
    client(message_queue *mq);
    virtual ~client();
    message_queue *queue();
    std::size_t qsize() const;
    receipt_map& outstanding();
    message_shared_ptr dequeue_message();
    message_id enqueue_message(unsigned char *data, std::size_t len);
    void wait();
    bool ack_message(message_id id);
    virtual void notify();
};


} // namespace tinymq

#endif /* CLIENT_HPP_ */
