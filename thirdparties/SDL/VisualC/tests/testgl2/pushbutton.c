#include <memory.h>
#include "pushbutton.h"
#include "font.h"

void DrawPushButtonToMemory(
	_Inout_ _Notnull_ unsigned char* surface,
	_In_ int sw,
	_In_ int sh,
	_In_ int x,
	_In_ int y,
	_In_ int w,
	_In_ int h,
	_In_ int bevelThickness,
	_In_ _Notnull_ const wchar_t* text,
	_In_ int fontsize
	)
{
	static const unsigned char FILL[4] = { 0x34, 0x34, 0x34, 255 };
	
	// 밝은 부분
	static const unsigned char NORTH[4] = { 0x56, 0x56, 0x56, 255 };
	static const unsigned char WEST[4] = { 0x56, 0x56, 0x56, 255 };
	
	// 어두운 부분
	static const unsigned char SOUTH[4] = { 0x14, 0x14, 0x14, 255 };
	static const unsigned char EAST[4] = { 0x14, 0x14, 0x14, 255 };

	int i, j;

	for (i = y; i < y + h; ++i)
	{
		for (j = x; j < x + w; ++j)
		{
			const int sidx = 4 * (sw * i + j);

			memcpy(surface + sidx, FILL, sizeof(FILL));
		}
	}

	for (i = y + h - bevelThickness; i < y + h; ++i)
	{
		for (j = x; j < x + w; ++j)
		{
			const int sidx = 4 * (sw * i + j);

			memcpy(surface + sidx, NORTH, sizeof(NORTH));
		}
	}

	for (i = y; i < y + bevelThickness; ++i)
	{
		for (j = x; j < x + w; ++j)
		{
			const int sidx = 4 * (sw * i + j);

			memcpy(surface + sidx, SOUTH, sizeof(SOUTH));
		}
	}

	for (i = y; i < y + h; ++i)
	{
		for (j = x; j < x + bevelThickness; ++j)
		{
			const int sidx = 4 * (sw * i + j);

			memcpy(surface + sidx, WEST, sizeof(WEST));
		}
	}

	for (i = y; i < y + h; ++i)
	{
		for (j = x + w - bevelThickness; j < x + w; ++j)
		{
			const int sidx = 4 * (sw * i + j);

			memcpy(surface + sidx, EAST, sizeof(EAST));
		}
	}

	DrawTextToMemory(
		surface,
		sw,
		sh,
		text,
		x + w/2 - GetTextWidth(fontsize, text)/2,
		y + h/2 + fontsize/2,
		fontsize
		);
}
