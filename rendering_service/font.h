#pragma once

#include <sal.h>

#ifdef RENDERING_SERVICE_EXPORTS
#define RENDERING_SERVICE_API __declspec(dllexport)
#else
#define RENDERING_SERVICE_API __declspec(dllimport)
#endif

RENDERING_SERVICE_API int InitializeFontSystem();

RENDERING_SERVICE_API int DrawGlyph(
    _In_ wchar_t c,
    _In_ int pixel_size,
    _Out_ _Notnull_ int* pitch,
    _Out_ _Notnull_ int* rows,
    _Out_ _Notnull_ int* advance,
    _Out_ _Notnull_ int* horiBearingY,
    _Inout_ _Notnull_ unsigned char** ppBitmap
    );

RENDERING_SERVICE_API void DrawTextToMemory(
    _Inout_ _Notnull_ unsigned char *const texdata,
    _In_ const int texwidth,
    _In_ const int texheight,
    _In_ _Notnull_ const wchar_t* text,
    _In_ int toffx,
    _In_ int toffy,
    _In_ const int fontsize,
    _In_opt_ float* tint
    );

RENDERING_SERVICE_API int GetTextWidth(
    _In_ int pixel_size,
    _In_ _Notnull_ const wchar_t* text,
    _Out_ _Notnull_ int* max_horiBearingY,
    _Out_ _Notnull_ int* max_height
    );
