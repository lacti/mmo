#pragma once

#include <boost/log/trivial.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>

#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <boost/log/attributes/timer.hpp>
#include <boost/log/attributes/named_scope.hpp>

#include <boost/log/sources/logger.hpp>

#include <boost/log/support/date_time.hpp>

// Here we define our application severity levels.
enum severity_level
{
    info,
	debug,
    error,
};

extern boost::log::sources::severity_logger< severity_level > slg;

#define LDBG_		BOOST_LOG_SEV(slg, debug)
#define LAPP_		BOOST_LOG_SEV(slg, info)
#define LERR_		BOOST_LOG_SEV(slg, error)

void init_logs_clientlogic();

void init_logs(
        const char* err_log_file_name = "err.txt", 
        const char* debug_log_file_name = "out.txt", 
		int enabled_level = boost::log::trivial::severity_level::debug
);
