#include "stdafx.h"
#include "basicdraw.h"

void DrawInvertedColorBoxToMemory(
	_In_ int sw,
	_In_ int sh,
	_Inout_ _Notnull_ unsigned char* surface,
	_In_ int px,
	_In_ int py,
	_In_ int w,
	_In_ int h)
{
	for (int i = py; i < py + h; ++i)
	{
		for (int j = px; j < px + w; ++j)
		{
			int sidx = 4*(sw*(sh-1-i) + j);

			if (sidx >= 0 && sidx < 4*sw*sh-3)
			{
				surface[ sidx + 0 ] = 255 - surface[ sidx + 0 ];
				surface[ sidx + 1 ] = 255 - surface[ sidx + 1 ];
				surface[ sidx + 2 ] = 255 - surface[ sidx + 2 ];
			}
		}
	}
}
