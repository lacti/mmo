#include "stdafx.h"
#include "simplestaticobject.h"
#include "objloader.h"
#include "dds.h"
#include "../mmobase/matrix.h"
#include "defaultshaderprogram.h"

static bool IsValidStaticObject(const SimpleStaticObject* obj)
{
    return !(obj == nullptr || obj->objFileName == nullptr || obj->objFileName[0] == '\0');
}

RENDERING_SERVICE_API int rs_load_simple_obj(SimpleStaticObject* obj)
{
    if (!IsValidStaticObject(obj))
    {
        return -1;
    }

    static std::unordered_map<std::string, GLuint> OnMemoryTextureResources;
    static std::unordered_map<std::string, VertexBuffer> OnMemoryStaticMeshResources;
    
    char resPath[1024];
    
    if (obj->objFileName)
    {
        auto vb = OnMemoryStaticMeshResources.find(obj->objFileName);
        if (vb == OnMemoryStaticMeshResources.end())
        {
            GetMmoResourcePath(resPath, _countof(resPath), obj->objFileName);
            load_obj(&obj->vb, resPath);

            OnMemoryStaticMeshResources[obj->objFileName] = obj->vb;
        }
        else
        {
            obj->vb = vb->second;
        }
    }

    if (obj->ddsFileName)
    {
        auto texId = OnMemoryTextureResources.find(obj->ddsFileName);
        if (texId == OnMemoryTextureResources.end())
        {
            GetMmoResourcePath(resPath, _countof(resPath), obj->ddsFileName);
            obj->texId = load_dds(resPath);

            OnMemoryTextureResources[obj->ddsFileName] = obj->texId;
        }
        else
        {
            obj->texId = texId->second;
        }
    }

    return 0;
}

RENDERING_SERVICE_API int rs_unload_simple_obj(SimpleStaticObject* obj)
{
    return 0;
}

RENDERING_SERVICE_API int rs_render_simple_obj(const SimpleStaticObject* obj, const SimpleTransform* xform)
{
    if (!IsValidStaticObject(obj) || !xform)
    {
        return -1;
    }

    CLSALIGN16 float m[16], rot44[16];
    CLSALIGN16 float xyzw[4] = { xform->x, xform->y, xform->z, 1.0f };
    CLSALIGN16 float scale[4] = { 3.0f, 3.0f, 5.0f/2, 1.0f };
    CLSALIGN16 float zaxis[4] = { 0.0f, 0.0f, 1.0f, 0.0f };

    glhLoadIdentityf2(m);
    glhTranslatef2_SSE_Aligned(m, xyzw);
    glhScalef2_SSE_Aligned(m, scale);
    //glhMultMatrixf2_SSE_Aligned(m, rot44);
    glhRotatef2_SSE_Aligned(m, (float)M_PI, zaxis);

    float uvx[9];
    glhLoadIdentity3f2(uvx);
    glUniformMatrix3fv(xform->uniUvx, 1, GL_FALSE, (GLfloat*)uvx);

    //SetUniMvp(m, v, p);

    glBindTexture(GL_TEXTURE_2D, obj->texId);
    glBindVertexArray(obj->vb.varray);
    glDrawArrays(GL_TRIANGLES, 0, obj->vb.len);
    glBindVertexArray(0);

    return 0;
}

typedef DefaultShaderProgram Prog;

RENDERING_SERVICE_API int rs_render_triangle_fan(unsigned int texId, unsigned int vbId, size_t vLen, const float* modelMat)
{
    Prog::Get()->SetModelMat(modelMat);
    Prog::Get()->UploadMvp();

    glBindTexture(GL_TEXTURE_2D, texId);
    glBindVertexArray(vbId);
    glDrawArrays(GL_TRIANGLE_FAN, 0, vLen);
    glBindVertexArray(0);

    return 0;
}

RENDERING_SERVICE_API int rs_render_triangles(unsigned int texId, unsigned int vbId, size_t vLen, const float* modelMat)
{
    Prog::Get()->SetModelMat(modelMat);
    Prog::Get()->UploadMvp();

    glBindTexture(GL_TEXTURE_2D, texId);
    glBindVertexArray(vbId);
    glDrawArrays(GL_TRIANGLES, 0, vLen);
    glBindVertexArray(0);

    return 0;
}
