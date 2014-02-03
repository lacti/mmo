#include "stdafx.h"
#include "font.h"

FT_Library library;
FT_Face face;

int InitializeFontSystem()
{
	int error;
	char resPath[1024];

	error = FT_Init_FreeType(&library);
	if (error)
	{
		return error;
	}

	
	GetMmoResourcePath(resPath, 1024, "go150.ttf");

	error = FT_New_Face(library, resPath, 0, &face);
	if (error == FT_Err_Unknown_File_Format)
	{
		return error;
	}
	else if (error)
	{
		return error;
	}
		
	return 0;
}

int DrawGlyph(
	_In_ wchar_t c,
	_In_ int pixel_size,
	_Out_ _Notnull_ int* pitch,
	_Out_ _Notnull_ int* rows,
	_Out_ _Notnull_ int* advance,
	_Out_ _Notnull_ int* horiBearingY,
	_Inout_ _Notnull_ unsigned char** ppBitmap
	)
{
	FT_Error error;
	FT_UInt glyph_index;

	glyph_index = FT_Get_Char_Index(face, c);

	error = FT_Set_Pixel_Sizes(face, 0, pixel_size);
	if (error)
	{
		return error;
	}

	error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
	if (error)
	{
		return error;
	}

	if (face->glyph->format == FT_GLYPH_FORMAT_OUTLINE)
	{
		FT_Pos strength = 30;
		error = FT_Outline_Embolden(&face->glyph->outline, strength);
		if (error)
		{
			//cl_debug_output("Font Embolden failed.");
            DebugBreak();
		}
	}

	/*
	FT_Matrix matrix;
	matrix.xx = 0x07FFFL;
	matrix.xy = 0;
	matrix.yx = 0;
	matrix.yy = 0x10000L;
	FT_Set_Transform(face, &matrix, 0);
	*/

	error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
	if (error)
	{
		return error;
	}

	*ppBitmap = (unsigned char*)malloc(face->glyph->bitmap.rows * face->glyph->bitmap.pitch);

	memcpy(*ppBitmap, face->glyph->bitmap.buffer,
		face->glyph->bitmap.rows * face->glyph->bitmap.pitch);

	*pitch = face->glyph->bitmap.pitch;
	*rows = face->glyph->bitmap.rows;
	*advance = face->glyph->advance.x / 64;
	*horiBearingY = face->glyph->metrics.horiBearingY / 64;

	return error;
}

void DrawTextToMemory(
	_Inout_ _Notnull_ unsigned char *const surface,
	_In_ const int sw,
	_In_ const int sh,
	_In_ _Notnull_ const wchar_t* text,
	_In_ int penx,
	_In_ int peny,
	_In_ const int fontsize,
	_In_opt_ float* tint
	)
{
	int ch, i, j;
	const int penx0 = penx;

	for (ch = 0; ch < (int)wcslen(text); ++ch)
	{
		unsigned char* bitmap = NULL;
		int pitch, rows, advance, horiBearingY;

		if (text[ch] == L'\n')
		{
			penx = penx0;
			peny += fontsize;
			continue;
		}

		if (DrawGlyph(text[ch], fontsize, &pitch, &rows, &advance, &horiBearingY, &bitmap))
		{
			// draw font failed
			free(bitmap);
			break;
		}

		for (i = 0; i < rows; ++i)
		{
			for (j = 0; j < pitch; ++j)
			{
				unsigned char glyphPixel = bitmap[pitch * i + j];
				
				const int pixelOffset = 4*(sw * (sh - 1 - (peny - horiBearingY + i)) + penx + j);
				
				if (glyphPixel && pixelOffset >= 0 && pixelOffset < 4*sw*sh-3)
				{
					int alphaacc = (int)surface[pixelOffset + 3] + glyphPixel;
					
					surface[pixelOffset + 0] = glyphPixel;
					surface[pixelOffset + 1] = glyphPixel;
					surface[pixelOffset + 2] = glyphPixel;
					surface[pixelOffset + 3] = alphaacc > 255 ? 255 : alphaacc;

					if (tint)
					{
						surface[pixelOffset + 0] = (unsigned char)(surface[pixelOffset + 0] * tint[0]);
						surface[pixelOffset + 1] = (unsigned char)(surface[pixelOffset + 1] * tint[1]);
						surface[pixelOffset + 2] = (unsigned char)(surface[pixelOffset + 2] * tint[2]);
					}
				}
			}
		}

		penx += advance;

		free(bitmap);
	}
}

int GetTextWidth(
	_In_ int pixel_size,
	_In_ _Notnull_ const wchar_t* text,
	_Out_ _Notnull_ int* max_horiBearingY,
	_Out_ _Notnull_ int* max_height
	)
{
	FT_Error error;
	FT_UInt glyph_index;
	int w = 0;
	size_t i;

	if (max_horiBearingY)
		*max_horiBearingY = 0;

	if (max_height)
		*max_height = 0;
	
	for (i = 0; i < wcslen(text); ++i)
	{
		wchar_t c = text[i];

		glyph_index = FT_Get_Char_Index(face, c);

		error = FT_Set_Pixel_Sizes(face, 0, pixel_size);
		if (error)
		{
			return 0;
		}

		error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
		if (error)
		{
			return 0;
		}

		w += face->glyph->advance.x;

		if (max_horiBearingY && *max_horiBearingY < face->glyph->metrics.horiBearingY)
			*max_horiBearingY = face->glyph->metrics.horiBearingY;

		if (max_height && *max_height < face->glyph->metrics.height)
			*max_height = face->glyph->metrics.height;
	}

	if (max_horiBearingY)
		*max_horiBearingY /= 64;

	if (max_height)
		*max_height /= 64;

	return w / 64;
}
