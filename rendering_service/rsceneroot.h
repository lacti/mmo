#pragma once
#include "rscenenode.h"
class RSceneRoot :
    public RSceneNode
{
public:
    static RHANDLE CreateSceneFromFbxFile(const char* pszFile);
    virtual RObjectType GetType() const { return RSCENEROOTTYPE; }
    
private:
    ~RSceneRoot(void);
};
