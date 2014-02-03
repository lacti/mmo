#pragma once

#ifdef RENDERING_SERVICE_EXPORTS
#define RENDERING_SERVICE_API __declspec(dllexport)
#else
#define RENDERING_SERVICE_API __declspec(dllimport)
#endif

#include "vertex.h"

struct SimpleStaticObject
{
    const char* objFileName;
    const char* ddsFileName;
    VertexBuffer vb;
    GLuint texId;
};

struct SimpleTransform
{
    float x, y, z;
    GLint uniUvx;
};

RENDERING_SERVICE_API int rs_load_simple_obj(SimpleStaticObject* obj);
RENDERING_SERVICE_API int rs_unload_simple_obj(SimpleStaticObject* obj);
RENDERING_SERVICE_API int rs_render_simple_obj(const SimpleStaticObject* obj);
RENDERING_SERVICE_API int rs_render_triangle_fan(unsigned int texId, unsigned int vbId, size_t vLen, const float* modelMat);
RENDERING_SERVICE_API int rs_render_triangles(unsigned int texId, unsigned int vbId, size_t vLen, const float* modelMat);
