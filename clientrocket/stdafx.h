// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <Windows.h>

// TODO: reference additional headers your program requires here
#include <Rocket/Core/SystemInterface.h>
#include <Rocket/Controls/DataSource.h>
#include <Rocket/Core/Types.h>
#include <Rocket/Core/ElementDocument.h>
#include <Rocket/Core/ElementUtilities.h>
#include <Rocket/Core/Input.h>

#define CLSALIGN16 __declspec(align(16))
