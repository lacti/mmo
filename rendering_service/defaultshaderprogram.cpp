#include "stdafx.h"
#include "defaultshaderprogram.h"
#include "shader_manager.h"
#include "vertex.h"
#include "../mmobase/matrix.h"
#include "../mmobase/basicmath.h"

void RsSetTextureMatrix(float* mvp);

DefaultShaderProgram* DefaultShaderProgram::instance;

void* DefaultShaderProgram::operator new(size_t size)
{
    void* storage = _aligned_malloc(size, 0x20);
    if (storage == nullptr)
    {
        throw "allocation fail: no free memory";
    }

    return storage;
}

void DefaultShaderProgram::operator delete(void* p)
{
    _aligned_free(p);
}

void DefaultShaderProgram::Initialize()
{
    if (instance == nullptr)
    {
        instance = new DefaultShaderProgram("sdl-default.vert", "sdl-default.frag");
    }
}

void DefaultShaderProgram::Finalize()
{
    delete instance;
    instance = nullptr;
}

DefaultShaderProgram::DefaultShaderProgram(const char* vsFileName, const char* fsFileName)
{
    char resPath[1024];
    GLuint vsId, fsId;
    GetMmoResourcePath(resPath, 1024, vsFileName);
    vsId = create_shader_from_file(GL_VERTEX_SHADER, resPath);

    GetMmoResourcePath(resPath, 1024, fsFileName);
    fsId = create_shader_from_file(GL_FRAGMENT_SHADER, resPath);

    m_programId = glCreateProgram();
    assert(m_programId);

    glAttachShader(m_programId, vsId);
    glAttachShader(m_programId, fsId);
    glDeleteShader(vsId);
    glDeleteShader(fsId);

    glBindAttribLocation(m_programId, POSITION, "Position");
    glBindAttribLocation(m_programId, TEXCOORD, "Texcoord");
    glBindAttribLocation(m_programId, NORMAL, "Normal");
    glBindAttribLocation(m_programId, COLOR, "Color");

    glLinkProgram(m_programId);

    const char* uniforms[] = {
        "MVP", "UVX", "Alpha", "DirLight", "MVPShadowMap", "ShadowMap"
    };

    for (auto&& c : uniforms)
    {
        m_uniforms[c] = glGetUniformLocation(m_programId, c);
    }

    // ShadowMap은 7번 텍스쳐에서 가져온다.
    glUseProgram(m_programId);
    glUniform1i(m_uniforms["ShadowMap"], 7);
}

void DefaultShaderProgram::UploadMvp()
{
    CLSALIGN16 GLfloat mvp[2][16];

    glhLoadIdentityf2(mvp[0]);
    glhMultMatrixSwappedf2_SSE_Aligned(mvp[0], m_modelMat);
    glhMultMatrixSwappedf2_SSE_Aligned(mvp[0], m_viewMat);
    glhMultMatrixSwappedf2_SSE_Aligned(mvp[0], m_projMat);

    // ...Shadow mapping...
    
    CLSALIGN16 float shadowBias[16] =
    {
        0.5, 0.0, 0.0, 0.0, 
        0.0, 0.5, 0.0, 0.0,
        0.0, 0.0, 0.5, 0.0,
        0.5, 0.5, 0.5, 1.0
    };
    glhLoadIdentityf2(mvp[1]);
    
    glhMultMatrixSwappedf2_SSE_Aligned(mvp[1], m_modelMat);
    glhMultMatrixSwappedf2_SSE_Aligned(mvp[1], m_shadowViewMat);
    glhMultMatrixSwappedf2_SSE_Aligned(mvp[1], m_shadowProjMat);
    glhMultMatrixSwappedf2_SSE_Aligned(mvp[1], shadowBias);

    // GPU로 업로드!
    glUniformMatrix4fv(m_uniforms["MVP"], 2, GL_FALSE, (const GLfloat*)mvp);
    //glUniformMatrix4fv(m_uniforms["MVPShadowMap"], 1, GL_FALSE, shadowMvp);
}

void DefaultShaderProgram::UploadAlpha(float a)
{
    glUniform1f(m_uniforms["Alpha"], a);
}

void DefaultShaderProgram::UploadLightDirection(const GLfloat* vec4)
{
    glUniform4fv(m_uniforms["DirLight"], 1, vec4);
}

void DefaultShaderProgram::UploadUvx(float u, float v, float ox, float oy)
{
    float uvx[9] = {
        u, 0, 0,
        0, v, 0,
        ox, oy, 1.0f
    };

    glUniformMatrix3fv(m_uniforms["UVX"], 1, GL_FALSE, uvx);
}

typedef DefaultShaderProgram Prog;

DefaultShaderProgram::~DefaultShaderProgram()
{
    glDeleteProgram(m_programId);
}

RENDERING_SERVICE_API void rs_init_default_shader_program()
{
    Prog::Initialize();
}

RENDERING_SERVICE_API void rs_set_model_matrix(const float* mat)
{
    Prog::Get()->SetModelMat(mat);
}

RENDERING_SERVICE_API void rs_set_view_matrix(const float* mat)
{
    Prog::Get()->SetViewMat(mat);
}

RENDERING_SERVICE_API void rs_set_proj_matrix(const float* mat)
{
    Prog::Get()->SetProjMat(mat);
}

RENDERING_SERVICE_API void rs_set_shadow_view_matrix(const float* mat)
{
    Prog::Get()->SetShadowViewMat(mat);
}

RENDERING_SERVICE_API void rs_set_shadow_proj_matrix(const float* mat)
{
    Prog::Get()->SetShadowProjMat(mat);
}

RENDERING_SERVICE_API void rs_set_light_direction(const float* vec4)
{
    Prog::Get()->UploadLightDirection(vec4);
}

RENDERING_SERVICE_API void rs_use_default_program()
{
    glUseProgram(Prog::Get()->GetProgramId());
}

RENDERING_SERVICE_API void rs_set_alpha(float a)
{
    Prog::Get()->UploadAlpha(a);
}

RENDERING_SERVICE_API void rs_set_uvx(float u, float v, float ox, float oy)
{
    Prog::Get()->UploadUvx(u, v, ox, oy);
}

RENDERING_SERVICE_API void rs_finalize_default_shader_program()
{
    Prog::Finalize();
}
