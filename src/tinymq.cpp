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

#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include "server.hpp"
//#include <boost/logging/format/named_write.hpp>
//#include <boost/logging/format_all.hpp>
//#include <boost/logging/format/formatter/tags.hpp>
//#include "tinymq_log.hpp"

namespace po = boost::program_options;
//using namespace boost::logging;

//BOOST_DEFINE_LOG_FILTER(g_l_filter, finder::filter ) 
//BOOST_DEFINE_LOG(g_l, finder::logger) 

/*
void init_logs() {
    g_l()->writer().add_formatter( formatter::tag::file_line(), "[ %] " );
    g_l()->writer().add_formatter( formatter::idx(), "[%] " );
    g_l()->writer().add_formatter( formatter::time("$hh:$mm.$ss.$mili ") );
    g_l()->writer().add_formatter( formatter::append_newline() );
    g_l()->writer().add_destination( destination::cout() );
    g_l()->turn_cache_off();
}
*/

int main(int argc, char* argv[]) {
    //    init_logs();
    std::string ip;
    std::string port;
    std::string datadir;
    po::options_description desc("usage: tinymq [options]");

    desc.add_options()
        ("help", "help message")
        ("ip,i", po::value<std::string>(&ip)->default_value("0.0.0.0"),
         "ip address to bind to")
        ("port,p", po::value<std::string>(&port)->default_value("5673"),
         "port number to listen on")
        ("dir,d", po::value<std::string>(&datadir)->default_value("."),
         "data directory");
    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
        if (vm.count("help")) {
            std::cout << desc << "\n";
            return 1;
        }
    }
    catch (po::error& e) {
        std::cerr << e.what() << "\n\n";
        std::cerr << desc << "\n";
        return 1;
    }
    try {
        //        LAPP_ << "starting tinymq.server (" << ip << ":" << port << ":" << datadir << ")";
        tinymq::server s(ip, port, datadir);
        s.run();
        s.stop();
        return 0;
    }
    catch (std::exception& e) {
        std::cerr << "exception: " << e.what() << "\n";
        return 1;
    }
}
