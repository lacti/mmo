#pragma once

#include <sal.h>
#include <wchar.h>

void
ShowSingleLineScreenText(
	_In_ const wchar_t* text,
	_In_ int textSize,
	_In_ int durationMs
	);

//void
//RegisterScreenText(
//	_In_ const unsigned char *const surface,
//	_In_ const int sw,
//	_In_ const int sh,
//	_In_ const int durationMs
//	);

void RenderScreenTexts(int w, int h, float dt);
