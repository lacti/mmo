#include "stdafx.h"
#include "RocketShaderProgram.h"
#include "shader_manager.h"
#include "vertex.h"
#include "../mmobase/matrix.h"

RocketShaderProgram* RocketShaderProgram::instance;

void* RocketShaderProgram::operator new(size_t size)
{
    void* storage = _aligned_malloc(size, 0x20);
    if (storage == nullptr)
    {
        throw "allocation fail: no free memory";
    }

    return storage;
}

void RocketShaderProgram::operator delete(void* p)
{
    _aligned_free(p);
}

void RocketShaderProgram::Initialize()
{
    if (instance == nullptr)
    {
        instance = new RocketShaderProgram("rocketlayer.vert", "rocketlayer.frag");
    }
}

void RocketShaderProgram::Finalize()
{
    delete instance;
    instance = nullptr;
}

RocketShaderProgram::RocketShaderProgram(const char* vsFileName, const char* fsFileName)
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
        "MVP", "TEX"
    };

    for (auto&& c : uniforms)
    {
        m_uniforms[c] = glGetUniformLocation(m_programId, c);
    }
}

void RocketShaderProgram::UploadMvp()
{
    CLSALIGN16 GLfloat mvp[16];

    glhLoadIdentityf2(mvp);
    glhMultMatrixSwappedf2_SSE_Aligned(mvp, m_modelMat);
    glhMultMatrixSwappedf2_SSE_Aligned(mvp, m_viewMat);
    glhMultMatrixSwappedf2_SSE_Aligned(mvp, m_projMat);

    glUniformMatrix4fv(m_uniforms["MVP"], 1, GL_FALSE, mvp);
}

void RocketShaderProgram::UploadEnableTex(bool b)
{
    glUniform1i(m_uniforms["TEX"], b ? 1 : 0);
}

typedef RocketShaderProgram Prog;

RocketShaderProgram::~RocketShaderProgram()
{
    glDeleteProgram(m_programId);
}

RENDERING_SERVICE_API void rs_init_rocket_shader_program()
{
    Prog::Initialize();
}

RENDERING_SERVICE_API void rs_set_rocket_model_matrix(const float* mat)
{
    Prog::Get()->SetModelMat(mat);
}

RENDERING_SERVICE_API void rs_set_rocket_view_matrix(const float* mat)
{
    Prog::Get()->SetViewMat(mat);
}

RENDERING_SERVICE_API void rs_set_rocket_proj_matrix(const float* mat)
{
    Prog::Get()->SetProjMat(mat);
}

RENDERING_SERVICE_API void rs_use_rocket_program()
{
    glUseProgram(Prog::Get()->GetProgramId());
}

RENDERING_SERVICE_API void rs_finalize_rocket_shader_program()
{
    Prog::Finalize();
}

RENDERING_SERVICE_API void rs_draw_2d_elements(Draw2DElementsParam* param)
{
    // 호출될 때마다 VAO, VBO 만들어서 한번 그리고 버린다.
    // 매우 매우 느리고 비효율적인 코드. 우선 돌아가게만 하고 나중에 개선시키자.

    VertexData3D* vd3d = new VertexData3D[param->num_vertices];
    for (size_t i = 0; i < param->num_vertices; ++i)
    {
        vd3d[i].x = *((float*)((BYTE*)param->vertex + param->stride * i) + 0);
        vd3d[i].y = *((float*)((BYTE*)param->vertex + param->stride * i) + 1);
        vd3d[i].z = 0;

        vd3d[i].nx = 0;
        vd3d[i].ny = 0;
        vd3d[i].nz = 1;

        if (param->texture && param->texCoord)
        {
            vd3d[i].u = *((float*)((BYTE*)param->texCoord + param->stride * i) + 0);
            vd3d[i].v = *((float*)((BYTE*)param->texCoord + param->stride * i) + 1);
        }
        else
        {
            vd3d[i].u = 0;
            vd3d[i].v = 0;
        }

        vd3d[i].r = *((BYTE*)param->color + param->stride * i + 0) / 255.0f;
        vd3d[i].g = *((BYTE*)param->color + param->stride * i + 1) / 255.0f;
        vd3d[i].b = *((BYTE*)param->color + param->stride * i + 2) / 255.0f;
        vd3d[i].a = *((BYTE*)param->color + param->stride * i + 3) / 255.0f;
    }
    
    VertexBuffer vb;
    vb.data = vd3d;
    vb.len = param->num_vertices;
    create_vertex_buffer_from_array_3d(&vb);
    delete [] vd3d;

    glBindTexture(GL_TEXTURE_2D, param->texture);
    glBindVertexArray(vb.varray);
    Prog::Get()->UploadMvp();
    Prog::Get()->UploadEnableTex(param->texture ? true : false);
    glDrawElements(GL_TRIANGLES, param->num_indices, GL_UNSIGNED_INT, param->indices);

    glDeleteVertexArrays(1, &vb.varray);
    glDeleteBuffers(1, &vb.vbuffer);
}
