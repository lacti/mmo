#pragma once

#ifdef MMOBASE_EXPORTS
#define MMOBASE_API __declspec(dllexport)
#else
#define MMOBASE_API __declspec(dllimport)
#endif

MMOBASE_API unsigned int nlpo2(unsigned int x);
MMOBASE_API float clampf(float v, float lo, float hi);
