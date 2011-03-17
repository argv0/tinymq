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

//#include <boost/random/linear_congruential.hpp>
//#include <boost/random/uniform_int.hpp>
//#include <boost/random/uniform_real.hpp>
//#include <boost/random/variate_generator.hpp>
//#include <boost/random/mersenne_twister.hpp>
#include <ctime>
#include "message.hpp"

namespace tinymq {

message::message(message_id id, message_len_t len, unsigned char* data,
        void_allocator_type *void_alloc) :
    header_(id, len),
    data_(*void_alloc) {
    data_.reserve(len);
    data_.insert(data_.begin(), data, data+len);
}

const message_id message::id() const {
    return header_.id;
}

const message_len_t message::size() const {
    return header_.len;
}

message::uchar_vector *message::data() {
    return &data_;
}

message_id_generator::message_id_generator() {
    counter = (uint64_t)std::time(0) << 32;
}

message_id message_id_generator::next_id() {
    return ++counter;
}

/*
uint16_t message_id_generator::random16() {
    base_generator_type generator(42u);
    generator.seed(static_cast<unsigned int>(std::time(0)));
    boost::uniform_int<> uni_dist(0,65535);
    boost::variate_generator<base_generator_type&,
    boost::uniform_int<> > uni(generator, uni_dist);
    return uni();
}
*/

} // namespace tinymq
