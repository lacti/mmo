#include "stdafx.h"
#include "pngloader.h"

static const size_t PNGSIGSIZE = 8;

bool validate(std::istream& source) {

    //Allocate a buffer of 8 bytes, where we can put the file signature.
    png_byte pngsig[PNGSIGSIZE];
    int is_png = 0;

    //Read the 8 bytes from the stream into the sig buffer.
    source.read((char*)pngsig, PNGSIGSIZE);

    //Check if the read worked...
    if (!source.good()) return false;

    //Let LibPNG check the sig. If this function returns 0, everything is OK.
    is_png = png_sig_cmp(pngsig, 0, PNGSIGSIZE);
    return (is_png == 0);
}

void userReadData(png_structp pngPtr, png_bytep data, png_size_t length) {
    //Here we get our IO pointer back from the read struct.
    //This is the parameter we passed to the png_set_read_fn() function.
    //Our std::istream pointer.
    png_voidp a = png_get_io_ptr(pngPtr);
    //Cast the pointer to std::istream* and read 'length' bytes into 'data'
    ((std::istream*)a)->read((char*)data, length);
}

GLuint create_texture_from_png_stream(std::istream& source, int& img_width, int& img_height)
{
    //so First, we validate our stream with the validate function I just mentioned
    if (!validate(source)) {
        std::cerr << "ERROR: Data is not valid PNG-data" << std::endl;
        return 0; //Do your own error recovery/handling here
    }

    //Here we create the png read struct. The 3 NULL's at the end can be used
    //for your own custom error handling functions, but we'll just use the default.
    //if the function fails, NULL is returned. Always check the return values!
    png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!pngPtr) {
        std::cerr << "ERROR: Couldn't initialize png read struct" << std::endl;
        return 0; //Do your own error recovery/handling here
    }

    //Here we create the png info struct.
    //Note that this time, if this function fails, we have to clean up the read struct!
    png_infop infoPtr = png_create_info_struct(pngPtr);
    if (!infoPtr) {
        std::cerr << "ERROR: Couldn't initialize png info struct" << std::endl;
        png_destroy_read_struct(&pngPtr, (png_infopp)0, (png_infopp)0);
        return 0; //Do your own error recovery/handling here
    }

    //Here I've defined 2 pointers up front, so I can use them in error handling.
    //I will explain these 2 later. Just making sure these get deleted on error.
    png_bytep* rowPtrs = NULL;

    png_set_sig_bytes(pngPtr, PNGSIGSIZE);

    //Now call png_read_info with our pngPtr as image handle, and infoPtr to receive the file info.
    png_set_read_fn(pngPtr,(void*)&source, userReadData);

    png_read_info(pngPtr, infoPtr);

    png_uint_32 imgWidth =  png_get_image_width(pngPtr, infoPtr);
    png_uint_32 imgHeight = png_get_image_height(pngPtr, infoPtr);

    //bits per CHANNEL! note: not per pixel!
    png_uint_32 bitdepth   = png_get_bit_depth(pngPtr, infoPtr);

    //Number of channels
    png_uint_32 channels   = png_get_channels(pngPtr, infoPtr);

    //Color type. (RGB, RGBA, Luminance, luminance alpha... palette... etc)

    png_uint_32 color_type = png_get_color_type(pngPtr, infoPtr);

    switch (color_type) {
        case PNG_COLOR_TYPE_PALETTE:
            png_set_palette_to_rgb(pngPtr);
            //Don't forget to update the channel info (thanks Tom!)
            //It's used later to know how big a buffer we need for the image
            channels = 3;           
            break;
    }

    RsDebugOutput("Width     = %d\n", imgWidth);
    RsDebugOutput("Height    = %d\n", imgHeight);
    RsDebugOutput("BitDepth  = %d\n", bitdepth);
    RsDebugOutput("Channels  = %d\n", channels);
    RsDebugOutput("ColorType = %d\n", color_type);

    /*if the image has a transperancy set.. convert it to a full Alpha channel..*/
    if (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS)) {
        png_set_tRNS_to_alpha(pngPtr);
        channels+=1;
    }

        //Here's one of the pointers we've defined in the error handler section:

    //Array of row pointers. One for every row.

    rowPtrs = new png_bytep[imgHeight];
    
    //Alocate a buffer with enough space.
    //(Don't use the stack, these blocks get big easilly)

    //This pointer was also defined in the error handling section, so we can clean it up on error.
    char* data = new char[imgWidth * imgHeight * bitdepth * channels / 8];

    //This is the length in bytes, of one row.
    const unsigned int stride = imgWidth * bitdepth * channels / 8;

    //A little for-loop here to set all the row pointers to the starting
    //Adresses for every row in the buffer

    for (size_t i = 0; i < imgHeight; i++) {
        //Set the pointer to the data pointer + i times the row stride.
        //Notice that the row order is reversed with q.
        //This is how at least OpenGL expects it,
        //and how many other image loaders present the data.
        size_t q = (imgHeight- i - 1) * stride;
        rowPtrs[i] = (png_bytep)data + q;
    }

    //And here it is! The actuall reading of the image!
    //Read the imagedata and write it to the adresses pointed to
    //by rowptrs (in other words: our image databuffer)
    png_read_image(pngPtr, rowPtrs);

    for (size_t i = 0; i < imgHeight; i++) {
        //rowPtrs[i]
    }

    GLuint Texture2DName = 0;
	glGenTextures(1, &Texture2DName);

    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Texture2DName);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST /*GL_LINEAR*/);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST /*GL_LINEAR_MIPMAP_LINEAR*/);

    glTexImage2D(
		GL_TEXTURE_2D, 
		0, 
		GL_RGBA, 
		GLsizei(imgWidth), 
		GLsizei(imgHeight), 
		0,  
		GL_RGBA, 
		GL_UNSIGNED_BYTE, 
		data);
     glGenerateMipmap(GL_TEXTURE_2D);


    //Delete the row pointers array....
    delete[] (png_bytep)rowPtrs;
    delete[] data;
    //And don't forget to clean up the read and info structs !
    png_destroy_read_struct(&pngPtr, &infoPtr,(png_infopp)0);

    img_width = imgWidth;
    img_height = imgHeight;

    return Texture2DName;
}
