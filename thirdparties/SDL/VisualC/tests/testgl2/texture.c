#include "SDL_opengl.h"

//extern PFNGLACTIVETEXTUREPROC glActiveTexture;
extern PFNGLGENERATEMIPMAPPROC glGenerateMipmap;

GLuint CreateTextureFromMemory(int imgWidth, int imgHeight, void* data)
{
    GLuint Texture2DName = 0;
	glGenTextures(1, &Texture2DName);

    //glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture2DName);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		GL_RGBA, 
		imgWidth, 
		imgHeight, 
		0,  
		GL_RGBA, 
		GL_UNSIGNED_BYTE, 
		data);
     glGenerateMipmap(GL_TEXTURE_2D);

	 return Texture2DName;
}
