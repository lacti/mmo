// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#define _SCL_SECURE_NO_WARNINGS
#include "targetver.h"

// Headers for CppUnitTest
#include "CppUnitTest.h"

// TODO: reference additional headers your program requires here
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/foreach.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/interprocess/detail/atomic.hpp>
#include <boost/interprocess/sync/interprocess_recursive_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/lexical_cast.hpp>

#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_queue.h>
#include <tbb/concurrent_hash_map.h>

#include <Windows.h>
#include <future>