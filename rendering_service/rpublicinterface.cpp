#include "stdafx.h"
#include "rpublicinterface.h"

#include "rsceneroot.h"
#include "rmeshdata.h"

RHANDLE r_load_fbx_scene(const char* pszFilePath)
{
    return RSceneRoot::CreateSceneFromFbxFile(pszFilePath);
}

size_t r_get_object_count(RObjectType type)
{
    return RObject::GetObjectCountOf(type);
}

RHANDLE r_find_handle(const char* pszName)
{
    return RObject::FindHandle(pszName);
}

size_t r_meshdata_vertex_count(RHANDLE r)
{
    RMeshData* ro = RMeshData::GetRObject(r);
    if (ro)
    {
        return ro->GetVertexCount();
    }

    return 0;
}

size_t r_unload_all()
{
    return RObject::UnloadAll();
}

const float* r_simplemesh_lcltranslation(RHANDLE r)
{
    RSceneNode* ro = RSceneNode::GetRObject(r);
    if (ro)
    {
        return ro->GetLclTranslation();
    }

    return nullptr;
}

const float* r_simplemesh_lclrotation(RHANDLE r)
{
    RSceneNode* ro = RSceneNode::GetRObject(r);
    if (ro)
    {
        return ro->GetLclRotation();
    }

    return nullptr;
}

const float* r_simplemesh_lclscaling(RHANDLE r)
{
    RSceneNode* ro = RSceneNode::GetRObject(r);
    if (ro)
    {
        return ro->GetLclScaling();
    }

    return nullptr;
}
