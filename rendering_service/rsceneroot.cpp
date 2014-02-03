#include "stdafx.h"
#include "rsceneroot.h"
#include "fbxsdk.h"
#include "fbxloader.h"
#include "../rendering_service/vertex.h"
#include "rscenenode.h"

RHANDLE RSceneRoot::CreateSceneFromFbxFile(const char* pszFile)
{
    FbxManager* ISdkManager = FbxManager::Create();
    FbxIOSettings *ios = FbxIOSettings::Create(ISdkManager, IOSROOT);
    ISdkManager->SetIOSettings(ios);

    FbxImporter* IImporter = FbxImporter::Create(ISdkManager, "");
    if (!IImporter->Initialize(pszFile, -1, ISdkManager->GetIOSettings()))
    {
        printf("Call to FbxImporter::Initialize() failed.\n");
        printf("Error returned: %s\n\n", IImporter->GetStatus().GetErrorString());

		ISdkManager->Destroy();
		return RHANDLE_INVALID;
    }

    FbxScene* IScene = FbxScene::Create(ISdkManager, "myScene");
    IImporter->Import(IScene);
    IImporter->Destroy(); IImporter = nullptr;

    std::vector<VertexBuffer> VbArray;

    auto pSceneRoot = new RSceneRoot();

    FbxNode* IRootNode = IScene->GetRootNode();
    if (IRootNode)
    {
        for (int i = 0; i < IRootNode->GetChildCount(); ++i)
        {
            auto handle = RSceneNode::CreateFromFbxNode(IRootNode->GetChild(i));
        }
    }

    ISdkManager->Destroy();

    return RObject::RegisterNewObject(pSceneRoot);
}

RSceneRoot::~RSceneRoot(void)
{
}
