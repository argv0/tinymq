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

#include "message_queue.hpp"
#include "tinymq_log.hpp"

namespace tinymq {

message_queue::message_queue(const char *mfile_name, std::size_t maxsize) :
    store_type(mfile_name, maxsize),
    valloc_(new base_t::void_allocator_type(get_segment_manager())),
    idgen_() {
    deque_ = find_or_construct<message_deque>("mq")(*valloc_);
}

void message_queue::add_waiter(client::shared_ptr client) {
    waiters_.insert(client::weak_ptr(client));
 }

std::size_t message_queue::qsize() const {
      return deque_->size();
}

void message_queue::notify_one() {
    if (waiters_.empty()) return;
    client::weak_ptr c(*waiters_.begin());
    if (!c.expired()) {
        client::shared_ptr cp =c.lock();
        cp->notify();
    }
    waiters_.erase(c);
}

void message_queue::grow(size_t by) {
    store_type::grow(by);
    valloc_ = boost::shared_ptr<base_t::void_allocator_type>(
                new base_t::void_allocator_type(get_segment_manager()));
    deque_ = find_or_construct<message_deque>("mq")(*valloc_);
}

message_id message_queue::enqueue_message(unsigned char *data, std::size_t len) {
    bool was_empty = (deque_->size() == 0);
    message_id id(idgen_.next_id());
    try {
        message_shared_ptr m = 
            bip::make_managed_shared_ptr
            (construct<message>(bip::anonymous_instance)(id, len, data, valloc_.get()),
             *this);
        deque_->push_back(m);
        if (was_empty) notify_one();
        return id;
    }
    catch (const bip::bad_alloc&) {
        grow(filesize());
        message_shared_ptr m = 
            bip::make_managed_shared_ptr(construct<message>(
                bip::anonymous_instance)(id, len, data, valloc_.get()),
                                         *this
            );
        deque_->push_back(m);
        if (was_empty) notify_one();
        return id;
    }
}

void message_queue::enqueue_message(message_shared_ptr m) {
    bool was_empty = (deque_->size() == 0);
    try {
        deque_->push_back(m);
        if (was_empty) notify_one();
    }
    catch (const bip::bad_alloc&) {
        grow(filesize());
        deque_->push_back(m);
        if (was_empty) notify_one();
    }
}

message_shared_ptr message_queue::dequeue_message() {
    if (deque_->size()) {
        message_shared_ptr m(deque_->front());
        deque_->pop_front();
        return m;
    }
    else {
        return message_shared_ptr();
    }
}
} // namespace tinymq
