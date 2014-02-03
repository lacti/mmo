#pragma once

#ifdef RENDERING_SERVICE_EXPORTS
#define RENDERING_SERVICE_API __declspec(dllexport)
CLSALIGN16 class RocketShaderProgram
{
public:
    static void Initialize();
    static void Finalize();
    static RocketShaderProgram* Get() { return instance; }

    void* operator new(size_t);
    void operator delete(void*);

    void SetModelMat(const GLfloat* mat) { memcpy(m_modelMat, mat, sizeof(m_modelMat)); }
    void SetViewMat(const GLfloat* mat) { memcpy(m_viewMat, mat, sizeof(m_viewMat)); }
    void SetProjMat(const GLfloat* mat) { memcpy(m_projMat, mat, sizeof(m_projMat)); }
    void UploadMvp();
    void UploadEnableTex(bool b);
    GLuint GetProgramId() const { return m_programId; }

private:
    RocketShaderProgram(const char* vsFileName, const char* fsFileName);
    RocketShaderProgram();
    RocketShaderProgram(const RocketShaderProgram&);
    ~RocketShaderProgram();

    CLSALIGN16 GLfloat m_modelMat[16];
    CLSALIGN16 GLfloat m_viewMat[16];
    CLSALIGN16 GLfloat m_projMat[16];

    static RocketShaderProgram* instance;

    GLuint m_programId;
    std::map<const char*, GLint> m_uniforms;
};
#else
#define RENDERING_SERVICE_API __declspec(dllimport)
#endif

struct Draw2DElementsParam
{
    size_t num_vertices;
    void* vertex;
    void* color;
    void* texCoord;
    size_t stride;
    GLuint texture;
    size_t num_indices;
    GLint* indices;
};

RENDERING_SERVICE_API void rs_init_rocket_shader_program();
RENDERING_SERVICE_API void rs_set_rocket_model_matrix(const float* mat);
RENDERING_SERVICE_API void rs_set_rocket_view_matrix(const float* mat);
RENDERING_SERVICE_API void rs_set_rocket_proj_matrix(const float* mat);
RENDERING_SERVICE_API void rs_finalize_rocket_shader_program();
RENDERING_SERVICE_API void rs_use_rocket_program();
RENDERING_SERVICE_API void rs_draw_2d_elements(Draw2DElementsParam* param);
