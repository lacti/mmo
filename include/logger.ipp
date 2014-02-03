#include "logger.h"

namespace logging = boost::log;
//namespace fmt = boost::log::formatters;
//namespace flt = boost::log::filters;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;

// The formatting logic for the severity level
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<< (
    std::basic_ostream< CharT, TraitsT >& strm, severity_level lvl)
{
    static const char* const str[] =
    {
        "app",
        "dbg",
        "err"
    };
    if (static_cast< std::size_t >(lvl) < (sizeof(str) / sizeof(*str)))
        strm << str[lvl];
    else
        strm << static_cast< int >(lvl);
    return strm;
}

src::severity_logger< severity_level > slg;


void init_logs_clientlogic()
{
	logging::core::get()->set_filter(
		logging::trivial::severity >= logging::trivial::info
		);

  //  logging::init_log_to_console(
		//std::clog, keywords::format = fmt::format("<%1%> %2%")
		//% fmt::attr< severity_level >("Severity", std::nothrow)
		//% fmt::message());

    //logging::add_common_attributes();
}

void init_logs(const char* err_log_file_name, const char* debug_log_file_name, int enabled_level)
{
	logging::add_console_log(std::clog, keywords::format = "%TimeStamp%: %Message%");

    logging::add_common_attributes();
	logging::core::get()->add_thread_attribute("Scope", attrs::named_scope());

	slg.add_attribute("Uptime", attrs::timer());
}
