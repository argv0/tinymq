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

#include "client.hpp"
#include "message_queue.hpp"

namespace tinymq {

client::client(message_queue *mq) :
    outstanding_(),
    queue_(mq) {}

client::~client() {
    for (receipt_map::iterator it=outstanding_.begin();
         it != outstanding_.end(); ++it) {
        queue_->enqueue_message(it->second.second);
    }
    outstanding_.clear();
}

std::size_t client::qsize() const {
    return queue_->qsize();
}

message_queue *client::queue() {
    return queue_;
}


client::receipt_map& client::outstanding() {
    return outstanding_;
}

message_id client::enqueue_message(unsigned char *data, std::size_t len) {
    return queue_->enqueue_message(data, len);
}

void client::wait() {
    queue_->add_waiter(shared_from_this());
}

message_shared_ptr client::dequeue_message() {
    message_shared_ptr m = queue_->dequeue_message();
    if (m) {
        message_receipt r(std::time(0), m);
        outstanding_[m->id()] = r;
    }
    return m;
}

bool client::ack_message(message_id id) {
    receipt_map::iterator it = outstanding_.find(id);
    if (it != outstanding_.end()) {
        outstanding_.erase(it);
        return true;
    }
    else {
        printf("ERROR: acking unknown message id %llu\n", id);
        return false;
    }
}

void client::notify() {

}

} // namespace tinymq
