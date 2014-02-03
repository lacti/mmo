#include "stdafx.h"
#include "texture.h"
#include "pngloader.h"
#include "font.h"
#include "../clientlogic/clientlogic.h"
using namespace client;

Texture Texture::create_from_file(const std::string& filename)
{
    Texture tex;

    std::ifstream png_stream(filename.c_str(), std::ios::binary);

    tex.name = create_texture_from_png_stream(png_stream, tex.width, tex.height);

    return tex;
}


unsigned int CreateBlankTexture(int w, int h)
{
    GLuint t;
    unsigned char* blank = (unsigned char*)calloc(4, w*h);

    t = CreateTextureFromMemory(w, h, blank);

    free(blank);

    return t;
}

unsigned int CreateTextureFromMemory(int sw, int sh, const unsigned char *const surface)
{
    GLuint Texture2DName = 0;
    glGenTextures(1, &Texture2DName);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture2DName);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST /*GL_LINEAR*/);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST /*GL_LINEAR_MIPMAP_LINEAR*/);

    glTexImage2D(
        GL_TEXTURE_2D, 
        0, 
        GL_RGBA, 
        sw, 
        sh, 
        0,  
        GL_RGBA, 
        GL_UNSIGNED_BYTE, 
        surface);
    glGenerateMipmap(GL_TEXTURE_2D);

    if (glGetError() != GL_NO_ERROR)
    {
        // TODO: Log
        printf("%s - %s", __FUNCTION__, (char*)glewGetErrorString(glGetError()));
    }

    return Texture2DName;
}
