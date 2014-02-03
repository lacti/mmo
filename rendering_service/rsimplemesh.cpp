#include "stdafx.h"
#include "rsimplemesh.h"

RHANDLE RSimpleMesh::Create(FbxNode* pFbxNode, RHANDLE MeshData)
{
    auto pSimpleMesh = new RSimpleMesh();
    pSimpleMesh->m_MeshDataHandle = MeshData;

    auto ro = RObject::FindRObject(MeshData);
    strcpy_s(pSimpleMesh->m_StringName, ro->GetStringName());
    strcat_s(pSimpleMesh->m_StringName, ".Instance");

    FbxVector4 v4 = pFbxNode->LclTranslation.Get();
    pSimpleMesh->m_LclTranslation[0] = static_cast<float>(v4[0]);
    pSimpleMesh->m_LclTranslation[1] = static_cast<float>(v4[1]);
    pSimpleMesh->m_LclTranslation[2] = static_cast<float>(v4[2]);

    v4 = pFbxNode->LclRotation.Get();
    pSimpleMesh->m_LclRotation[0] = static_cast<float>(v4[0]);
    pSimpleMesh->m_LclRotation[1] = static_cast<float>(v4[1]);
    pSimpleMesh->m_LclRotation[2] = static_cast<float>(v4[2]);

    v4 = pFbxNode->LclScaling.Get();
    pSimpleMesh->m_LclScaling[0] = static_cast<float>(v4[0]);
    pSimpleMesh->m_LclScaling[1] = static_cast<float>(v4[1]);
    pSimpleMesh->m_LclScaling[2] = static_cast<float>(v4[2]);

    return RObject::RegisterNewObject(pSimpleMesh);
}

RSimpleMesh::~RSimpleMesh(void)
{
}
