#include "stdafx.h"
#include "rscenenode.h"
#include "fbxsdk.h"
#include "rmeshdata.h"
#include "rsimplemesh.h"

RHANDLE RSceneNode::CreateFromFbxNode(FbxNode* pFbxNode)
{
    if (_stricmp(pFbxNode->GetTypeName(), "mesh") == 0)
    {
        auto dataHandle = RMeshData::CreateFromFbxNode(pFbxNode);
        return RSimpleMesh::Create(pFbxNode, dataHandle);
    }

    return RHANDLE_INVALID;
}

RSceneNode::~RSceneNode(void)
{
}

void RSceneNode::AddChild(RHANDLE r)
{
}

void RSceneNode::RemoveChild(RHANDLE r)
{
}
