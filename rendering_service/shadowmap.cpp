#include "stdafx.h"
#include "shadowmap.h"
#include "shader_manager.h"

RENDERING_SERVICE_API void rs_generateShadowFBO(shadowmap_conf* conf)
{
    //GLfloat borderColor[4] = {0,0,0,0};

    GLenum FBOstatus;

    // Try to use a texture depth component
    glGenTextures(1, &conf->depthTextureId);
    //glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, conf->depthTextureId);

    // GL_LINEAR does not make sense for depth texture. However, next tutorial shows usage of GL_LINEAR and PCF
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Remove artefact on the edges of the shadowmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );

    //glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );



    // No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available 
    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, conf->shadowMapWidth, conf->shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // create a framebuffer object
    glGenFramebuffers(1, &conf->fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, conf->fboId);

    // Instruct openGL that we won't bind a color texture with the currently binded FBO
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // attach the texture to FBO depth attachment point
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, conf->depthTextureId, 0);

    // check FBO status
    FBOstatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(FBOstatus != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("GL_FRAMEBUFFER_COMPLETE failed, CANNOT use FBO\n");
#ifdef _DEBUG
        DebugBreak();
#endif
    }

    // switch back to window-system-provided framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// Loading shader function
static GLhandleARB loadShader(char* filename, unsigned int type)
{
    FILE *pfile;
    GLhandleARB handle;
    const GLcharARB* files[1];

    // shader Compilation variable
    GLint result;				// Compilation code result
    GLint errorLoglength ;
    char* errorLogText;
    GLsizei actualErrorLogLength;

    char buffer[400000];
    memset(buffer,0,400000);

    // This will raise a warning on MS compiler
    pfile = fopen(filename, "rb");
    if(!pfile)
    {
        printf("Sorry, can't open file: '%s'.\n", filename);

        //throw std::runtime_error("shader file open error");
        return 0;
    }

    fread(buffer,sizeof(char),400000,pfile);
    //printf("%s\n",buffer);


    fclose(pfile);

    handle = glCreateShaderObjectARB(type);
    if (!handle)
    {
        //We have failed creating the vertex shader object.
        printf("Failed creating vertex shader object from file: %s.",filename);
        exit(0);
    }

    files[0] = (const GLcharARB*)buffer;
    glShaderSourceARB(
        handle, //The handle to our shader
        1, //The number of files.
        files, //An array of const char * data, which represents the source code of theshaders
        NULL);

    glCompileShaderARB(handle);

    //Compilation checking.
    glGetObjectParameterivARB(handle, GL_OBJECT_COMPILE_STATUS_ARB, &result);

    // If an error was detected.
    if (!result)
    {
        //We failed to compile.
        printf("Shader '%s' failed compilation.\n",filename);

        //Attempt to get the length of our error log.
        glGetObjectParameterivARB(handle, GL_OBJECT_INFO_LOG_LENGTH_ARB, &errorLoglength);

        //Create a buffer to read compilation error message
        errorLogText = (char*)malloc(sizeof(char) * errorLoglength);

        //Used to get the final length of the log.
        glGetInfoLogARB(handle, errorLoglength, &actualErrorLogLength, errorLogText);

        // Display errors.
        printf("%s\n",errorLogText);

        // Free the buffer malloced earlier
        free(errorLogText);
    }

    return handle;
}

RENDERING_SERVICE_API void rs_build_shadowmap_shader_program(shadowmap_conf* conf)
{
    //GLhandleARB vertexShaderHandle;
    //GLhandleARB fragmentShaderHandle;

    char resPath[1024];

    GetMmoResourcePath(resPath, 1024, "shadowmap.vert");
    //vertexShaderHandle   = loadShader(resPath, GL_VERTEX_SHADER);
    GLuint vertexShaderHandle = create_shader_from_file(GL_VERTEX_SHADER, resPath);
    if (vertexShaderHandle == 0)
    {
        return;
    }

    GetMmoResourcePath(resPath, 1024, "shadowmap.frag");
    //fragmentShaderHandle = loadShader(resPath,GL_FRAGMENT_SHADER);
    GLuint fragmentShaderHandle = create_shader_from_file(GL_FRAGMENT_SHADER, resPath);
    if (fragmentShaderHandle == 0)
    {
        return;
    }

    conf->shadowShaderId = glCreateProgram();

    glAttachShader(conf->shadowShaderId,vertexShaderHandle);
    glAttachShader(conf->shadowShaderId,fragmentShaderHandle);
    glLinkProgram(conf->shadowShaderId);

    conf->shadowMapUniform = glGetUniformLocation(conf->shadowShaderId,"ShadowMap");

    return;
}

void RsSetTextureMatrix(float* mvp)
{
    // This is matrix transform every coordinate x,y,z
    // x = x* 0.5 + 0.5 
    // y = y* 0.5 + 0.5 
    // z = z* 0.5 + 0.5 
    // Moving from unit cube [-1,1] to [0,1]  
    const GLdouble bias[16] = {
        0.5, 0.0, 0.0, 0.0, 
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0};

    glMatrixMode(GL_TEXTURE);
    glActiveTextureARB(GL_TEXTURE7);

    glLoadIdentity();
    glLoadMatrixd(bias);

    // concatating all matrice into one.
    glMultMatrixf(mvp);

    // Go back to normal matrix mode
    glMatrixMode(GL_MODELVIEW);

    glActiveTextureARB(GL_TEXTURE0);
}
