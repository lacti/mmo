#pragma once

#include <sal.h>

void DrawInvertedColorBoxToMemory(
	_In_ int sw,
	_In_ int sh,
	_Inout_ _Notnull_ unsigned char* surface,
	_In_ int px,
	_In_ int py,
	_In_ int w,
	_In_ int h);
