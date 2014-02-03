#include "stdafx.h"

#if 0

#include "fbxsdk.h"
#include "fbxloader.h"
#include "vertex.h"

static int numTabs = 0;

static void PrintTabs()
{
    for (int i = 0; i < numTabs; ++i)
    {
        printf("\t");
    }
}

static FbxString GetAttributeTypeName(FbxNodeAttribute::EType type) { 
    switch(type) { 
    case FbxNodeAttribute::eUnknown: return "unidentified"; 
    case FbxNodeAttribute::eNull: return "null"; 
    case FbxNodeAttribute::eMarker: return "marker"; 
    case FbxNodeAttribute::eSkeleton: return "skeleton"; 
    case FbxNodeAttribute::eMesh: return "mesh"; 
    case FbxNodeAttribute::eNurbs: return "nurbs"; 
    case FbxNodeAttribute::ePatch: return "patch"; 
    case FbxNodeAttribute::eCamera: return "camera"; 
    case FbxNodeAttribute::eCameraStereo: return "stereo"; 
    case FbxNodeAttribute::eCameraSwitcher: return "camera switcher"; 
    case FbxNodeAttribute::eLight: return "light"; 
    case FbxNodeAttribute::eOpticalReference: return "optical reference"; 
    case FbxNodeAttribute::eOpticalMarker: return "marker"; 
    case FbxNodeAttribute::eNurbsCurve: return "nurbs curve"; 
    case FbxNodeAttribute::eTrimNurbsSurface: return "trim nurbs surface"; 
    case FbxNodeAttribute::eBoundary: return "boundary"; 
    case FbxNodeAttribute::eNurbsSurface: return "nurbs surface"; 
    case FbxNodeAttribute::eShape: return "shape"; 
    case FbxNodeAttribute::eLODGroup: return "lodgroup"; 
    case FbxNodeAttribute::eSubDiv: return "subdiv"; 
    default: return "unknown"; 
    } 
}

static void PrintAttribute(FbxNodeAttribute* pAttribute) {
    if(!pAttribute) return;

    FbxString typeName = GetAttributeTypeName(pAttribute->GetAttributeType());
    FbxString attrName = pAttribute->GetName();
    PrintTabs();
    // Note: to retrieve the character array of a FbxString, use its Buffer() method.
    printf("<attribute type='%s' name='%s'/>\n", typeName.Buffer(), attrName.Buffer());
}

int GetTriCount(const FbxMesh* pMesh)
{
    if (pMesh == nullptr) return 0;

    int nRet = 0;

    for (int i = 0; i < pMesh->GetPolygonCount(); ++i)
    {
        if (pMesh->GetPolygonSize(i) == 3)
        {
            nRet += 1;
        }
        else if (pMesh->GetPolygonSize(i) == 4)
        {
            nRet += 2;
        }
    }

    return nRet;
}

void ProcessTriangle(VertexData3D* f, const FbxMesh* pMesh, const int* v, const int* u)
{
    auto pNormals = pMesh->GetLayer(0)->GetNormals();
    auto pUVs = pMesh->GetLayer(0)->GetUVs();

    for (int i = 0; i < 3; ++i)
    {
        const auto& pP = pMesh->GetControlPoints()[v[i]];
        const auto& pN = pNormals->GetDirectArray().GetAt(v[i]);

        static const double NULL_UVS[2] = {0,0};

        const auto& pU = pUVs ? pUVs->GetDirectArray().GetAt(u[i]) : NULL_UVS;

        f[i].x = static_cast<float>(pP[0]);
        f[i].y = static_cast<float>(pP[1]);
        f[i].z = static_cast<float>(pP[2]);
        
        f[i].u = pU[0];
        f[i].v = pU[1];

        f[i].nx = pN[0];
        f[i].ny = pN[1];
        f[i].nz = pN[2];

        f[i].r = 1.0f;
        f[i].g = 1.0f;
        f[i].b = 1.0f;
        f[i].a = 1.0f;
    }
}

static void ProcessNode(VertexBuffer* pVb, FbxNode* pNode)
{
    if (_stricmp(pNode->GetTypeName(), "mesh") == 0)
    {
        //pVb->
        auto pMesh = (FbxMesh*)pNode->GetNodeAttribute();

        assert(pMesh->GetLayerCount() == 1);
        auto pNormals = pMesh->GetLayer(0)->GetNormals();
        assert(pNormals->GetReferenceMode() == FbxLayerElement::eDirect);
        auto pUVs = pMesh->GetLayer(0)->GetUVs();
        assert(pUVs->GetReferenceMode() == FbxLayerElement::eIndexToDirect);
        assert(pUVs->GetMappingMode()  == FbxGeometryElement::eByPolygonVertex);

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

        pVb->data = farray;
        pVb->len = nf;
    }
}

static void PrintNode(FbxNode* pNode)
{
    PrintTabs();

    const char* nodeName = pNode->GetName();
    FbxDouble3 translation = pNode->LclTranslation.Get();
    FbxDouble3 rotation = pNode->LclRotation.Get();
    FbxDouble3 scaling = pNode->LclScaling.Get();

    printf("%s <%s>, (%.2f,%.2f,%.2f), (%.2f,%.2f,%.2f), (%.2f,%.2f,%.2f)\n",
        nodeName,
        pNode->GetTypeName(),
        translation[0], translation[1], translation[2],
        rotation[0], rotation[1], rotation[2],
        scaling[0], scaling[1], scaling[2]);

    //if (stricmp(pNode->GetTypeName(), "mesh") == 0)
    //{
    //    auto pMesh = (FbxMesh*)pNode->GetNodeAttribute();
    //    int numCp = pMesh->GetControlPointsCount();

    //    int numLay = pMesh->GetLayerCount();

    //    int numPoly = pMesh->GetPolygonCount();

    //    assert(pMesh->GetLayerCount() == 1);
    //    auto pNormals = pMesh->GetLayer(0)->GetNormals();
    //    assert(pNormals->GetReferenceMode() == FbxLayerElement::eDirect);
    //    
    //    auto pUVs = pMesh->GetLayer(0)->GetUVs();
    //    
    //    for (int i = 0; i < numPoly; ++i)
    //    {
    //        printf("Polygon [%d]\n", i);

    //        int nPolySize = pMesh->GetPolygonSize(i);
    //        for (int j = 0; j < nPolySize; ++j)
    //        {
    //            int k = pMesh->GetPolygonVertex(i, j);

    //            const auto& pV = pMesh->GetControlPoints()[k];
    //            printf("    [%d] : %.2f,%.2f,%.2f,%.2f\n", j, pV[0], pV[1], pV[2], pV[3]);
    //            
    //            const auto& pN = pNormals->GetDirectArray().GetAt(k);
    //            printf("          n%.2f,%.2f,%.2f,%.2f\n", pN[0], pN[1], pN[2], pN[3]);

    //            const auto& pU = pUVs->GetDirectArray().GetAt(k);
    //            printf("          u%.2f,%.2f\n", pU[0], pU[1]);
    //        }
    //    }

    //    FbxTime lTime;
    //    lTime.SetSecondDouble(1.0);

    //    // Get the node's global transformation matrix at 2 seconds.
    //    FbxAMatrix& lGlobalTransform = pNode->EvaluateGlobalTransform(lTime);
    //    const auto& trans = pNode->EvaluateLocalTranslation(lTime);

    //    printf("Pos at 1.0sec = %.2f,%.2f,%.2f,%.2f\n", trans[0], trans[1], trans[2], trans[3]);

    //    //auto ILayer = pMesh->GetLayer(0);

    //    //ILayer->

    //    printf("%d %d\n", numCp, numLay);
    //}

    //FbxAMatrix& lLocalTransform = pNode->EvaluateLocalTransform();

    ++numTabs;

    for(int i = 0; i < pNode->GetNodeAttributeCount(); i++)
    {
        PrintAttribute(pNode->GetNodeAttributeByIndex(i));
    }

    for (int j = 0; j < pNode->GetChildCount(); ++j)
    {
        PrintNode(pNode->GetChild(j));
    }

    --numTabs;
}

RENDERING_SERVICE_API void rs_test_load_fbx_file()
{
    FbxManager* ISdkManager = FbxManager::Create();
    FbxIOSettings *ios = FbxIOSettings::Create(ISdkManager, IOSROOT);
    ISdkManager->SetIOSettings(ios);

    FbxImporter* IImporter = FbxImporter::Create(ISdkManager, "");
    if (!IImporter->Initialize("C:\\Users\\Ben\\Documents\\test.fbx", -1, ISdkManager->GetIOSettings()))
    {
        printf("Call to FbxImporter::Initialize() failed.\n");
        printf("Error returned: %s\n\n", IImporter->GetStatus().GetErrorString());
    }

    FbxScene* IScene = FbxScene::Create(ISdkManager, "myScene");
    IImporter->Import(IScene);
    IImporter->Destroy();

    FbxNode* IRootNode = IScene->GetRootNode();
    if (IRootNode)
    {
        for (int i = 0; i < IRootNode->GetChildCount(); ++i)
        {
            PrintNode(IRootNode->GetChild(i));
        }
    }

    ISdkManager->Destroy();
}

RENDERING_SERVICE_API void rs_load_fbx(VertexBuffer** pVbArray, int* pVbArrayNum, const char* filename)
{
    if (pVbArray == nullptr || pVbArrayNum == nullptr || filename == nullptr)
    {
        return;
    }

    FbxManager* ISdkManager = FbxManager::Create();
    FbxIOSettings *ios = FbxIOSettings::Create(ISdkManager, IOSROOT);
    ISdkManager->SetIOSettings(ios);

    FbxImporter* IImporter = FbxImporter::Create(ISdkManager, "");
    if (!IImporter->Initialize(filename, -1, ISdkManager->GetIOSettings()))
    {
        printf("Call to FbxImporter::Initialize() failed.\n");
        printf("Error returned: %s\n\n", IImporter->GetStatus().GetErrorString());
    }

    FbxScene* IScene = FbxScene::Create(ISdkManager, "myScene");
    IImporter->Import(IScene);
    IImporter->Destroy(); IImporter = nullptr;

    std::vector<VertexBuffer> VbArray;

    FbxNode* IRootNode = IScene->GetRootNode();
    if (IRootNode)
    {
        for (int i = 0; i < IRootNode->GetChildCount(); ++i)
        {
            VertexBuffer Vb;
            memset(&Vb, 0, sizeof(VertexBuffer));

            ProcessNode(&Vb, IRootNode->GetChild(i));

            if (Vb.len > 0)
            {
                VbArray.push_back(Vb);
            }
        }
    }

    ISdkManager->Destroy();

    *pVbArray = new VertexBuffer[VbArray.size()];
    *pVbArrayNum = VbArray.size();

    for (auto& vb : VbArray)
    {
        create_vertex_buffer_from_array_3d(&vb);
    }

    for (int i = 0; i < VbArray.size(); ++i)
    {
        memcpy((*pVbArray) + i, &VbArray[i], sizeof(VertexBuffer));
    }    
}

#endif
