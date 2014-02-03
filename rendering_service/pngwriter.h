#ifndef __PNGWRITER_H_
#define __PNGWRITER_H_

enum WPF_ERROR_CODE
{
    WPF_OKAY,
    WPF_FILE_OPEN,
    WPF_LIBPNG,

};

extern "C"
{
__declspec(dllexport) int write_png_file_rgba8(const char* file_name, int width, int height,
    unsigned char** row_pointers);
}


#endif