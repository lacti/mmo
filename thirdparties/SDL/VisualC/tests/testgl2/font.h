#pragma once

int InitializeFontSystem();

int DrawGlyph(
	_In_ wchar_t c,
	_In_ int pixel_size,
	_Out_ _Notnull_ int* pitch,
	_Out_ _Notnull_ int* rows,
	_Out_ _Notnull_ int* advance,
	_Out_ _Notnull_ int* horiBearingY,
	_Inout_ _Notnull_ char** ppBitmap
	);

void DrawTextToMemory(
	_Inout_ _Notnull_ char *const texdata,
	_In_ const int texwidth,
	_In_ const int texheight,
	_In_ _Notnull_ const wchar_t* text,
	_In_ int toffx,
	_In_ int toffy,
	_In_ const int fontsize
	);

int GetTextWidth(
	_In_ int pixel_size,
	_In_ _Notnull_ const wchar_t* text
	);
