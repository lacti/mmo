#pragma once

unsigned int
CreateSigninWindowBackgroundTexture(
	_Inout_ _Notnull_ unsigned char** ppSurface
	);

void
RedrawSigninWindow(
	_In_ _Notnull_ const unsigned char *const bgSurface,
	_In_ unsigned int texId,
	_In_ _Notnull_ const wchar_t* userName,
	_In_ _Notnull_ const wchar_t* worldName
	);

unsigned int
CreateChatWindowBackgroundTexture(
	_Inout_ _Notnull_ unsigned char** ppSurface,
	_In_ int texsize_x,
	_In_ int texsize_y
	);

void
RedrawChatWindow(
	_In_ _Notnull_ const unsigned char *const bgSurface,
	_In_ unsigned int texId,
	_In_ _Notnull_ const wchar_t* chatLog,
	_In_ _Notnull_ const wchar_t* currentLine,
	_In_ int inputFocus
	);
