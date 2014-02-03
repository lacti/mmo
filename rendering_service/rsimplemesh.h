#pragma once
#include "rscenenode.h"
class RSimpleMesh :
    public RSceneNode
{
public:
    static RHANDLE Create(FbxNode* pFbxNode, RHANDLE MeshData);
    virtual RObjectType GetType() const { return RSIMPLEMESHTYPE; }
    virtual const char* GetStringName() const { return m_StringName; }

private:
    ~RSimpleMesh(void);

    char m_StringName[128];
    RHANDLE m_MeshDataHandle;
};
