#pragma once

#include <sal.h>

void DrawPushButtonToMemory(
	_Inout_ _Notnull_ unsigned char *const surface,
	_In_ int sw,
	_In_ int sh,
	_In_ int x,
	_In_ int y,
	_In_ int w,
	_In_ int h,
	_In_ int bevelThickness,
	_In_ _Notnull_ const wchar_t* text,
	_In_ int fontsize
	);
