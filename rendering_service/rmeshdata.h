#pragma once
#include "robject.h"
#include "vertex.h"

class RMeshData :
    public RObject
{
public:
    static RHANDLE CreateFromFbxNode(FbxNode* pFbxNode);
    virtual RObjectType GetType() const { return RMESHDATATYPE; }
    virtual const char* GetStringName() const { return m_StringName; }
    static RMeshData* GetRObject(RHANDLE r) { return static_cast<RMeshData*>(RObject::FindRObject(r)); }
    size_t GetVertexCount() const { return m_VertexBuffer.len; }

private:
    ~RMeshData(void);

    char m_StringName[128];
    VertexBuffer m_VertexBuffer;
};
