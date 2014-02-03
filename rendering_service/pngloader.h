#ifndef __PNGLOADER_H_
#define __PNGLOADER_H_

GLuint create_texture_from_png_stream(std::istream& source, int& img_width, int& img_height);

#endif