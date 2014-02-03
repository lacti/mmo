#include "stdafx.h"

const char* string_from_text_file(const char* filename)
{
	HANDLE handle;
	LARGE_INTEGER s;
	char* text;
	DWORD read;

	handle = CreateFileA(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);

	GetFileSizeEx(handle, &s);

	if (s.HighPart != 0)
		return NULL;
    
	text = (char*)malloc(s.LowPart + 1);
	
	ReadFile(handle, text, s.LowPart, &read, NULL);

	CloseHandle(handle);

	text[s.LowPart] = '\0';
     
    return text;
}
