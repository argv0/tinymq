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

#ifndef MESSAGE_QUEUE_HPP_
#define MESSAGE_QUEUE_HPP_

#include <set>
#include <boost/interprocess/containers/deque.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/shared_ptr.hpp>
#include "message.hpp"
#include "client.hpp"
#include "durable_store.hpp"

namespace bip = boost::interprocess;

namespace tinymq {

class message_queue : public virtual store<> { 
public:
    typedef store<> base_t;
    typedef bip::allocator<message_shared_ptr, base_t::segment_manager_type> message_allocator;
    typedef bip::deque<message_shared_ptr, message_allocator> message_deque;
private:
    typedef store<> store_type;
    boost::shared_ptr<base_t::void_allocator_type> valloc_;
    message_id_generator idgen_;
    message_deque *deque_;
    std::set<client::weak_ptr> waiters_;
public: // construction/destruction
    message_queue(const char *mfile_name, std::size_t maxsize);
public: // api 
    virtual void       grow(std::size_t by);
    message_id         enqueue_message(unsigned char *data, std::size_t len);
    void               enqueue_message(message_shared_ptr m);
    message_shared_ptr dequeue_message();
    void               add_waiter(client::shared_ptr client);
    void               notify_one();
    std::size_t        qsize() const ;
};

} // namespace tinymq

#endif // include guard
