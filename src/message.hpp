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

#ifndef MESSAGE_HPP_
#define MESSAGE_HPP_

#include <cstddef>
#include <vector>
#include <stdint.h>
#include <stdexcept>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/smart_ptr/shared_ptr.hpp>
#include <boost/interprocess/smart_ptr/weak_ptr.hpp>
#include "durable_store.hpp"

namespace tinymq {

namespace bip = boost::interprocess;

typedef uint64_t message_id;
typedef uint64_t message_len_t;

struct message_header {
    message_id id;
    message_len_t len;
    message_header(message_id id, message_len_t len) :
        id(id),
        len(len) {}
};

class message {
public:
    typedef store<> store_type;
    typedef store_type::segment_manager_type segment_manager_type;
    typedef store_type::void_allocator_type void_allocator_type;
    typedef bip::allocator<unsigned char, segment_manager_type> uchar_allocator;
    typedef bip::vector<unsigned char, uchar_allocator>      uchar_vector;
    typedef bip::allocator<uchar_vector, segment_manager_type>  uchar_vector_allocator;
private:
    message_header header_;
    uchar_vector data_;
public:
    message(message_id id, message_len_t len, unsigned char* data,
            void_allocator_type *void_alloc);
    const message_header header() const { return header_; }
    const message_id id() const;
    const message_len_t size() const;
    uchar_vector* data();
};

class message_id_generator {
    //typedef boost::mt19937 base_generator_type;
    message_id counter;
public:
    message_id_generator();
    message_id next_id();
};

typedef bip::managed_shared_ptr<message, message::store_type>::type message_shared_ptr;
typedef bip::managed_weak_ptr<message, message::store_type>::type message_weak_ptr;

} // namespace tinymq

#endif // include guard
