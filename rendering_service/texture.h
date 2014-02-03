#ifndef __TEXTURE_H_
#define __TEXTURE_H_

#ifdef RENDERING_SERVICE_EXPORTS
#define RENDERING_SERVICE_API __declspec(dllexport)
#else
#define RENDERING_SERVICE_API __declspec(dllimport)
#endif

namespace client {
    class Texture
    {
    public:
        static Texture create_from_file(const std::string&);

        GLuint name;

        int width;
        int height;
    };
}

unsigned int
CreateBlankTexture(
	int w,
	int h
	);

RENDERING_SERVICE_API
unsigned int
CreateTextureFromMemory(
	int sw,
	int sh,
	const unsigned char *const surface
	);

void
UpdateDynamicText(
	_In_ _Notnull_ unsigned long* dirtyCount,
	_In_ _Notnull_ wchar_t* imeText,
	_In_ _Notnull_ wchar_t* compositingText,
	_In_ _Notnull_ unsigned char* bgSurface,
	unsigned int texId,
	int x,
	int y
	);

#endif
