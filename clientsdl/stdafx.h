#pragma once

#define _SCL_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include <io.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <tchar.h>
#include <strsafe.h>

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <unordered_map>

#include <boost/bind.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/program_options.hpp>
#include <boost/range/adaptors.hpp>

#include <tbb/concurrent_vector.h>
#include <tbb/concurrent_queue.h>

#include <GL/glew.h>

#include <SDL.h>
#include <SDL_video.h>
#include <SDL_audio.h>
#include <SDL_opengl.h>

#include <Rocket/Core.h>
#include <Rocket/Controls.h>
#include <Rocket/Debugger.h>
#include <ShellRenderInterfaceOpenGL.h>
#include <ShellSystemInterface.h>
#include <Shell.h>
#include <Input.h>
#include <Rocket/Core/SystemInterface.h>
#include <Rocket/Controls/DataSource.h>
#include <Rocket/Core/Types.h>

#include <ode/ode.h>

#include <lame.h>
#include <lame_main.h>

#include "../include/utf8.h"
#include "../mmobase/mmobase.h"
#include "../clientlogic/clientlogic.h"
#include "../clientlogic/event.h"
#include "../rendering_service/rendering_service.h"
//#include "../rendering_service/shadowmap.h"

#define CLSALIGN16 __declspec(align(16))

inline wchar_t* SDL_create_wide_str(const char* utf8_str)
{
    return reinterpret_cast<wchar_t*>(SDL_iconv_string("UCS-2-INTERNAL", "UTF-8", utf8_str, (SDL_strlen(utf8_str)+1)*sizeof(char)));
}
