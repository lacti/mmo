#include "stdafx.h"
#include <iostream>
#include "shader_manager.h"
#include "file_util.h"
#include "rendering_service.h"

using namespace client;

ShaderManager::ShaderManager()
{
    printf("ShaderManager constructed.\n");
}

ShaderManager::~ShaderManager()
{
    printf("ShaderManager destructed.\n");
}

GLuint create_shader_from_file(GLenum type, const std::string& filename)
{
    std::string shader_text = string_from_text_file(filename);
    if (!shader_text.empty()) {
        GLuint name = glCreateShader(type);
        const char * shader_text_ptr = shader_text.c_str();
        glShaderSource(name, 1, &shader_text_ptr, 0);
        glCompileShader(name);
        return name;
    }

    return 0;
}

void ShaderManager::register_shader_program(const std::string& vs_filename, const std::string& fs_filename)
{   
    GLuint vs_name = create_shader_from_file(GL_VERTEX_SHADER, vs_filename);
    GLuint fs_name = create_shader_from_file(GL_FRAGMENT_SHADER, fs_filename);

    std::cout << "vs_name = " << vs_name << std::endl;
    std::cout << "fs_name = " << fs_name << std::endl;

    program_name = glCreateProgram();
    glAttachShader(program_name, vs_name);
    glAttachShader(program_name, fs_name);
    glDeleteShader(vs_name);
    glDeleteShader(fs_name);

    glBindAttribLocation(program_name, semantic::attr::POSITION, "Position");
    glBindAttribLocation(program_name, semantic::attr::TEXCOORD, "Texcoord");
    glLinkProgram(program_name);
    GLint link_ok;
    glGetProgramiv(program_name, GL_LINK_STATUS, &link_ok);
    if (!link_ok)
    {
        fprintf(stderr, "!!!!!!!!!!! glLinkProgram error.\n");
    }
    glValidateProgram(program_name);
    GLint validate_ok;
    glGetProgramiv(program_name, GL_VALIDATE_STATUS, &validate_ok);
    if (!validate_ok)
    {
        fprintf(stderr, "!!!!!!!!!!! glValidateProgram error.\n");
    }

    u_mvp		 = glGetUniformLocation(program_name, "MVP");
    u_diffuse	 = glGetUniformLocation(program_name, "Diffuse");
    u_tile_count = glGetUniformLocation(program_name, "TileCount");
    u_tile_index = glGetUniformLocation(program_name, "TileIndex");
    u_tile_size  = glGetUniformLocation(program_name, "TileSize");
    u_is_font_vs = glGetUniformLocation(program_name, "IsFontVs");
    u_is_font_fs = glGetUniformLocation(program_name, "IsFontFs");
    u_is_outline_fs = glGetUniformLocation(program_name, "IsOutlineFs");
    u_alpha_add	 = glGetUniformLocation(program_name, "AlphaAdd");
    u_alpha_mult = glGetUniformLocation(program_name, "AlphaMult");

    RsDebugOutput("u_mvp        : %d\n", u_mvp);
    RsDebugOutput("u_diffuse    : %d\n", u_diffuse);
    RsDebugOutput("u_tile_count : %d\n", u_tile_count);
    RsDebugOutput("u_tile_index : %d\n", u_tile_index);
    RsDebugOutput("u_tile_size  : %d\n", u_tile_size);
    RsDebugOutput("u_is_font_vs : %d\n", u_is_font_vs);
    RsDebugOutput("u_is_font_fs : %d\n", u_is_font_fs);
    RsDebugOutput("u_is_outline_fs : %d\n", u_is_outline_fs);
    RsDebugOutput("u_alpha_add  : %d\n", u_alpha_add);
    RsDebugOutput("u_alpha_mult : %d\n", u_alpha_mult);

    GLenum e;
    e = glGetError();

    const char* a_name = "v_coord";
    a_v_coord = glGetAttribLocation(program_name, a_name);
    if (a_v_coord == -1)
    {
        fprintf(stderr, "GLSL Link: Attribute 변수 %s 바인딩에 실패했습니다.\n", a_name);
    }
    e = glGetError();
    a_name = "v_test";
    GLint a_v_test = glGetAttribLocation(program_name, a_name);
    if (a_v_test == -1)
    {
        fprintf(stderr, "GLSL Link: Attribute 변수 %s 바인딩에 실패했습니다.\n", a_name);
    }
    e = glGetError();
    const char* u_name = "fbo_texture";
    u_fbo_texture = glGetUniformLocation(program_name, u_name);
    if (u_fbo_texture == -1)
    {
        fprintf(stderr, "GLSL Link: Uniform 변수 %s 바인딩에 실패했습니다.\n", u_name);
    }
    e = glGetError();
    u_name = "v_coord_scale";
    a_v_coord_scale = glGetUniformLocation(program_name, u_name);
    if (a_v_coord_scale == -1)
    {
        fprintf(stderr, "GLSL Link: Uniform 변수 %s 바인딩에 실패했습니다.\n", u_name);
    }

    e = glGetError();
}
