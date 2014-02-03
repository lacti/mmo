#pragma once
#include "robject.h"

#if 0

class RSceneNode :
    public RObject
{
public:
    static RHANDLE CreateFromFbxNode(FbxNode* pFbxNode);
    void AddChild(RHANDLE r);
    void RemoveChild(RHANDLE r);
    const float* GetLclTranslation() const { return m_LclTranslation; }
    const float* GetLclRotation() const { return m_LclRotation; }
    const float* GetLclScaling() const { return m_LclScaling; }
    static RSceneNode* GetRObject(RHANDLE r) { return static_cast<RSceneNode*>(RObject::FindRObject(r)); }

protected:
    ~RSceneNode(void);
    float m_LclTranslation[3];
    float m_LclRotation[3];
    float m_LclScaling[3];

private:
    std::vector<RHANDLE> m_Children;
};

#endif
