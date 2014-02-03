#include "stdafx.h"
#include "rmeshdata.h"
#include "fbxsdk.h"

int GetTriCount(const FbxMesh* pMesh);
void ProcessTriangle(VertexData3D* f, const FbxMesh* pMesh, const int* v, const int* u);

RHANDLE RMeshData::CreateFromFbxNode(FbxNode* pFbxNode)
{
    assert(_stricmp(pFbxNode->GetTypeName(), "mesh") == 0);
    if (_stricmp(pFbxNode->GetTypeName(), "mesh") != 0)
    {
        return RHANDLE_INVALID;
    }

    auto pMesh = static_cast<FbxMesh*>(pFbxNode->GetNodeAttribute());

    assert(pMesh->GetLayerCount() == 1);
    auto pNormals = pMesh->GetLayer(0)->GetNormals();
    assert(pNormals->GetReferenceMode() == FbxLayerElement::eDirect);
    auto pUVs = pMesh->GetLayer(0)->GetUVs();
    if (pUVs != nullptr)
    {
        assert(pUVs->GetReferenceMode() == FbxLayerElement::eIndexToDirect);
        assert(pUVs->GetMappingMode()  == FbxGeometryElement::eByPolygonVertex);
    }

    auto IMaterialElement = pMesh->GetElementMaterial();
    if (IMaterialElement != nullptr)
    {
        assert(IMaterialElement->GetReferenceMode() == FbxLayerElement::eIndexToDirect);
        //assert(IMaterialElement->GetMappingMode() == FbxGeometryElement::eByPolygon);
        assert(IMaterialElement->GetMappingMode() == FbxGeometryElement::eAllSame);
    }

    VertexData3D* farray = (VertexData3D*)malloc(sizeof(VertexData3D)*3*GetTriCount(pMesh));
    int nf = 0;
        
    for (int i = 0; i < pMesh->GetPolygonCount(); ++i)
    {
        if (pMesh->GetPolygonSize(i) == 3)
        {
            const int v[3] = {
                pMesh->GetPolygonVertex(i, 0),
                pMesh->GetPolygonVertex(i, 1),
                pMesh->GetPolygonVertex(i, 2)
            };
            const int u[3] = {
                pMesh->GetTextureUVIndex(i, 0),
                pMesh->GetTextureUVIndex(i, 1),
                pMesh->GetTextureUVIndex(i, 2)
            };
            ProcessTriangle(&farray[nf], pMesh, v, u);
            nf += 3;
        }
        else if (pMesh->GetPolygonSize(i) == 4)
        {
            const int v1[3] = {
                pMesh->GetPolygonVertex(i, 0),
                pMesh->GetPolygonVertex(i, 1),
                pMesh->GetPolygonVertex(i, 2)
            };
            const int u1[3] = {
                pMesh->GetTextureUVIndex(i, 0),
                pMesh->GetTextureUVIndex(i, 1),
                pMesh->GetTextureUVIndex(i, 2)
            };
            ProcessTriangle(&farray[nf], pMesh, v1, u1);
            nf += 3;

            const int v2[3] = {
                pMesh->GetPolygonVertex(i, 2),
                pMesh->GetPolygonVertex(i, 3),
                pMesh->GetPolygonVertex(i, 0)
            };
            const int u2[3] = {
                pMesh->GetTextureUVIndex(i, 2),
                pMesh->GetTextureUVIndex(i, 3),
                pMesh->GetTextureUVIndex(i, 0)
            };
            ProcessTriangle(&farray[nf], pMesh, v2, u2);
            nf += 3;
        }
    }

    auto pMeshData = new RMeshData();
    memset(&pMeshData->m_VertexBuffer, 0, sizeof(pMeshData->m_VertexBuffer));
    pMeshData->m_VertexBuffer.data = farray;
    pMeshData->m_VertexBuffer.len = nf;
    strcpy_s(pMeshData->m_StringName, pFbxNode->GetName());

    return RObject::RegisterNewObject(pMeshData);
}

RMeshData::~RMeshData(void)
{
}
