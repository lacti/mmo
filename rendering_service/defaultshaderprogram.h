#pragma once

#ifdef RENDERING_SERVICE_EXPORTS
#define RENDERING_SERVICE_API __declspec(dllexport)
CLSALIGN16 class DefaultShaderProgram
{
public:
    static void Initialize();
    static void Finalize();
    static DefaultShaderProgram* Get() { return instance; }

    void* operator new(size_t);
    void operator delete(void*);

    void SetModelMat(const GLfloat* mat) { memcpy(m_modelMat, mat, sizeof(m_modelMat)); }
    void SetViewMat(const GLfloat* mat) { memcpy(m_viewMat, mat, sizeof(m_viewMat)); }
    void SetProjMat(const GLfloat* mat) { memcpy(m_projMat, mat, sizeof(m_projMat)); }
    void SetShadowViewMat(const GLfloat* mat) { memcpy(m_shadowViewMat, mat, sizeof(m_shadowViewMat)); }
    void SetShadowProjMat(const GLfloat* mat) { memcpy(m_shadowProjMat, mat, sizeof(m_shadowProjMat)); }
    void UploadLightDirection(const GLfloat* vec4);
    void UploadMvp();
    void UploadAlpha(float a);
    void UploadUvx(float u, float v, float ox, float oy);
    GLuint GetProgramId() const { return m_programId; }

private:
    DefaultShaderProgram(const char* vsFileName, const char* fsFileName);
    DefaultShaderProgram();
    DefaultShaderProgram(const DefaultShaderProgram&);
    ~DefaultShaderProgram();

    CLSALIGN16 GLfloat m_modelMat[16];
    CLSALIGN16 GLfloat m_viewMat[16];
    CLSALIGN16 GLfloat m_projMat[16];
    CLSALIGN16 GLfloat m_shadowViewMat[16];
    CLSALIGN16 GLfloat m_shadowProjMat[16];

    static DefaultShaderProgram* instance;

    GLuint m_programId;
    std::map<const char*, GLint> m_uniforms;
};
#else
#define RENDERING_SERVICE_API __declspec(dllimport)
#endif

RENDERING_SERVICE_API void rs_init_default_shader_program();
RENDERING_SERVICE_API void rs_set_model_matrix(const float* mat);
RENDERING_SERVICE_API void rs_set_view_matrix(const float* mat);
RENDERING_SERVICE_API void rs_set_proj_matrix(const float* mat);
RENDERING_SERVICE_API void rs_set_shadow_view_matrix(const float* mat);
RENDERING_SERVICE_API void rs_set_shadow_proj_matrix(const float* mat);
RENDERING_SERVICE_API void rs_set_light_direction(const float* vec4);
RENDERING_SERVICE_API void rs_use_default_program();
RENDERING_SERVICE_API void rs_set_alpha(float a);
RENDERING_SERVICE_API void rs_set_uvx(float u, float v, float ox, float oy);
RENDERING_SERVICE_API void rs_finalize_default_shader_program();
