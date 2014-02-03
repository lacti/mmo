#pragma once

#define CLSALIGN16 __declspec(align(16))

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// Windows Header Files:
#include <windows.h>

// TODO: reference additional headers your program requires here
#include <memory>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <cassert>
#include <fstream>
#include <unordered_map>
#include <png.h>
#include <cml/cml.h>
#include <GL/glew.h>

#include <ft2build.h>
#include <freetype/ftoutln.h>
#include <freetype/ftglyph.h>
#include FT_FREETYPE_H

#include "../mmobase/mmobase.h"

namespace semantic {
    namespace attr {
        enum type {
            POSITION = 0,
            COLOR = 3,
            TEXCOORD = 4
        };
    }
    namespace direction {
        enum type {
            LEFT, RIGHT, UP, DOWN, NUM
        };
    }
}

#ifndef _countof
#define _countof(arr) (sizeof(arr)/sizeof(arr[0]))
#endif

//extern boost::filesystem::path g_work_path;

inline std::string get_file(const char* f)
{
	//return (g_work_path / f).string();
	std::string s;
	s.resize(1024);
	GetMmoResourcePath(&s[0], 1024, f);
    return s;
}

typedef cml::matrix44f_c matrix;

void RsDebugOutput(const char* format, ...);

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795029
#endif

#ifdef RENDERING_SERVICE_EXPORTS
#define RENDERING_SERVICE_API __declspec(dllexport)
#else
#define RENDERING_SERVICE_API __declspec(dllimport)
#endif
