#ifndef TINYMQ_LOG_HPP_
#define TINYMQ_LOG_HPP_

#include <boost/logging/format_fwd.hpp>

namespace bl = boost::logging;
typedef bl::level::holder filter_type;
typedef bl::tag::holder< bl::optimize::cache_string_one_str<>, 
                         bl::tag::file_line, 
                         bl::tag::function,
                         bl::tag::thread_id, 
                         bl::tag::time> log_string;
BOOST_LOG_FORMAT_MSG( log_string )

using namespace boost::logging::scenario::usage;
typedef use<
        filter_::change::single_thread, 
        filter_::level::use_levels, 
        logger_::change::single_thread, 
        logger_::favor::correctness> finder;

BOOST_DECLARE_LOG_FILTER(g_l_filter, finder::filter ) 
BOOST_DECLARE_LOG(g_l, finder::logger) 

#define LDBG_ BOOST_LOG_USE_LOG_IF_LEVEL(g_l(), g_l_filter(), debug ) .set_tag( BOOST_LOG_TAG_FILELINE)
#define LERR_ BOOST_LOG_USE_LOG_IF_LEVEL(g_l(), g_l_filter(), error ) .set_tag( BOOST_LOG_TAG_FILELINE)
#define LAPP_ BOOST_LOG_USE_LOG_IF_LEVEL(g_l(), g_l_filter(), info ) .set_tag( BOOST_LOG_TAG_FILELINE)

void init_logs();

#endif
