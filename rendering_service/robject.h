#pragma once

#include "rpublictypes.h"

class RObject
{
public:
    ~RObject(void);
    virtual RObjectType GetType() const = 0;
    virtual const char* GetStringName() const { return ""; }

    static size_t GetObjectCountOf(RObjectType type);
    static RHANDLE RegisterNewObject(RObject* pObject);
    static RHANDLE FindHandle(const char* pszName);
    static size_t UnloadAll();

protected:
    RObject(void);
    static RHANDLE IssueNewObjectHandle();
    static std::map<RHANDLE, RObject*> Instances;
    //template <typename T> T* AllocateNewObject();
    static RObject* FindRObject(RHANDLE r);

private:
    RObject(const RObject&);
    RObject(const RObject&&);
};
