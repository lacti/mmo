#include "stdafx.h"
#include "rendering_service.h"
#include "shader_manager.h"
#include "defaultshaderprogram.h"

GLuint fbo, fbo_texture, rbo_depth;
GLuint vbo_fbo_vertices;
client::ShaderManager* gShaderManager;
float fullscreen_backbuffer_scale = 1.0f;
GLuint gSquareMeshVao;

// 전체 이미지 렌더링 될 백 버퍼 생성
RENDERING_SERVICE_API void rs_create_backbuffer(int screen_width, int screen_height)
{
    GLenum e;
    e = glGetError();

    // Color Texture
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &fbo_texture);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        screen_width,
        screen_height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        NULL);
    glBindTexture(GL_TEXTURE_2D, 0);
    e = glGetError();

    // Depth buffer
    glGenRenderbuffers(1, &rbo_depth);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_depth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screen_width, screen_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Framebuffer to link everything together
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo_depth);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);
    GLenum status;
    if ((status = glCheckFramebufferStatus(GL_FRAMEBUFFER)) != GL_FRAMEBUFFER_COMPLETE)
    {
        fprintf(stderr, "glCheckFramebufferStatus: error %p", status);
        return;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 전체화면 덮는 사각형 생성
    glGenVertexArrays(1, &gSquareMeshVao);
    glBindVertexArray(gSquareMeshVao);
    glGenBuffers(1, &vbo_fbo_vertices);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
    const GLfloat fbo_vertices[] = {
        -1, -1,
         1, -1,
        -1,  1,
         1,  1
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(fbo_vertices), fbo_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(gShaderManager->a_v_coord);
    glVertexAttribPointer(gShaderManager->a_v_coord, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

RENDERING_SERVICE_API void rs_free_backbuffer()
{
    glDeleteRenderbuffers(1, &rbo_depth);
    glDeleteTextures(1, &fbo_texture);
    glDeleteFramebuffers(1, &fbo);

    glDeleteBuffers(1, &vbo_fbo_vertices);
}

RENDERING_SERVICE_API void rs_create_pp_program()
{
    char vsPath[1024], fsPath[1024];
    GetMmoResourcePath(vsPath, 1024, "postproc.v.vert");
    GetMmoResourcePath(fsPath, 1024, "postproc.v.frag");
    
    gShaderManager = new client::ShaderManager();
    gShaderManager->register_shader_program(vsPath, fsPath);
}

RENDERING_SERVICE_API void rs_start_backbuffer_rendering()
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

RENDERING_SERVICE_API void rs_render_fullscreen_backbuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(1.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(gShaderManager->program_name);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    glUniform1i(gShaderManager->u_fbo_texture, /*GL_TEXTURE*/0);
    glUniform1f(gShaderManager->a_v_coord_scale, fullscreen_backbuffer_scale);

    

    glBindBuffer(GL_ARRAY_BUFFER, vbo_fbo_vertices);
    glBindVertexArray(gSquareMeshVao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

RENDERING_SERVICE_API float rs_get_fullscreen_backbuffer_scale()
{
    return fullscreen_backbuffer_scale;
}

RENDERING_SERVICE_API void rs_set_fullscreen_backbuffer_scale(float s)
{
    fullscreen_backbuffer_scale = s;
}
