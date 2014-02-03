#include <ft2build.h>
#include FT_FREETYPE_H

FT_Library library;
FT_Face face;

int InitializeFontSystem()
{
	int error;
	
	error = FT_Init_FreeType(&library);
	if (error)
	{
		return error;
	}

	error = FT_New_Face(library, "malgun.ttf", 0, &face);
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
	_Inout_ _Notnull_ char** ppBitmap
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

	error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);
	if (error)
	{
		return error;
	}

	*ppBitmap = (char*)malloc(face->glyph->bitmap.rows * face->glyph->bitmap.pitch);

	memcpy(*ppBitmap, face->glyph->bitmap.buffer,
		face->glyph->bitmap.rows * face->glyph->bitmap.pitch);

	*pitch = face->glyph->bitmap.pitch;
	*rows = face->glyph->bitmap.rows;
	*advance = face->glyph->advance.x / 64;
	*horiBearingY = face->glyph->metrics.horiBearingY / 64;

	return error;
}

void DrawTextToMemory(
	_Inout_ _Notnull_ char *const texdata,
	_In_ const int texwidth,
	_In_ const int texheight,
	_In_ _Notnull_ const wchar_t* text,
	_In_ int toffx,
	_In_ int toffy,
	_In_ const int fontsize
	)
{
	int ch, i, j;

	for (ch = 0; ch < (int)wcslen(text); ++ch)
	{
		char* bitmap;
		int pitch, rows, advance, horiBearingY;

		DrawGlyph(text[ch], fontsize, &pitch, &rows, &advance, &horiBearingY, &bitmap);

		for (i = 0; i < rows; ++i)
		{
			for (j = 0; j < pitch; ++j)
			{
				if (bitmap[(rows-1-i) * pitch + j])
				{
					const int pixelIdx = 4*(texwidth * (toffy - fontsize - (rows - horiBearingY) + i) + toffx + j);
					const char glyphPixel = bitmap[(rows-1-i) * pitch + j];

					int alphaacc = (int)texdata[pixelIdx + 3] + (int)glyphPixel;

					texdata[pixelIdx + 0] = glyphPixel;
					texdata[pixelIdx + 1] = glyphPixel;
					texdata[pixelIdx + 2] = glyphPixel;

					texdata[pixelIdx + 3] = alphaacc > 255 ? 255 : alphaacc;
					
				}
			}
		}

		toffx += advance;

		free(bitmap);
	}
}

int GetTextWidth(
	_In_ int pixel_size,
	_In_ _Notnull_ const wchar_t* text
	)
{
	FT_Error error;
	FT_UInt glyph_index;
	int w = 0;
	size_t i;
	
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
	}

	return w / 64;
}
