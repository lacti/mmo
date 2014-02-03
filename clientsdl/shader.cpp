#include "stdafx.h"
#include "file.h"

unsigned int create_shader_from_file(GLenum type, const char* filename)
{
    const char* shader_text_ptr = string_from_text_file(filename);
    GLuint name;

    if (shader_text_ptr == NULL)
        return 0;

    //puts(shader_text_ptr);

    name = glCreateShader(type);

    glShaderSource(name, 1, &shader_text_ptr, 0);
    glCompileShader(name);

    free((void*)shader_text_ptr);

    GLint result;				// Compilation code result
    GLint errorLoglength ;
    char* errorLogText;
    GLsizei actualErrorLogLength;
    //Compilation checking.
    glGetObjectParameterivARB(name, GL_OBJECT_COMPILE_STATUS_ARB, &result);

    // If an error was detected.
    if (!result)
    {
        //We failed to compile.
        printf("Shader '%s' failed compilation.\n",filename);

        //Attempt to get the length of our error log.
        glGetObjectParameterivARB(name, GL_OBJECT_INFO_LOG_LENGTH_ARB, &errorLoglength);

        //Create a buffer to read compilation error message
        errorLogText = (char*)malloc(sizeof(char) * errorLoglength);

        //Used to get the final length of the log.
        glGetInfoLogARB(name, errorLoglength, &actualErrorLogLength, errorLogText);

        // Display errors.
        printf("%s\n",errorLogText);

        // Free the buffer malloced earlier
        free(errorLogText);
    }

    return name;
}
