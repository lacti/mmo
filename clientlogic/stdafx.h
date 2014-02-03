// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <strsafe.h>

#include <iostream>
#include <vector>
#include <map>
#include <cassert>
#include <cmath>
#include <algorithm>
#include <functional>
#include <thread>
#include <unordered_map>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <boost/interprocess/detail/atomic.hpp>
#include <boost/interprocess/sync/interprocess_recursive_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/lexical_cast.hpp>

#include <tinyxml.h>
#ifndef WIN32
typedef long long __int64;
#endif

#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_queue.h>
#include <tbb/concurrent_hash_map.h>

#include <btBulletDynamicsCommon.h>

#include <utf8.h>

#include "singleton.h"
#include "../mmobase/mmobase.h"

typedef unsigned int portable_size_t;

void ClDebugOutput(const char* format, ...);
void ClChatOutput(int id, const char* line);

namespace client { class entity; }

// PLUG: 컴포넌트 플러깅 단축 함수
template <typename T> void PLUG(const client::entity& e) { ClCtxEm()->plug<T>(e); }

// C: 컴포넌트 가져오기 단축 함수
template <typename T> T& C(const client::entity& e) { return ClCtxEm()->get_as<T>(e); }

// H: 컴포넌트 플러깅 여부 확인 단축 함수
template <typename T> bool H(const client::entity& e) { return ClCtxEm()->has<T>(e) > 0; }
