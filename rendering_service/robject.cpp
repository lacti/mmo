#include "stdafx.h"
#include "robject.h"

std::map<RHANDLE, RObject*> RObject::Instances;

RObject::RObject(void)
{
}


RObject::~RObject(void)
{
}

RHANDLE RObject::IssueNewObjectHandle()
{
    static RHANDLE r = RHANDLE_INVALID + 1;

    ++r;

    return r - 1;
}

size_t RObject::GetObjectCountOf(RObjectType type)
{
    int c = 0;

    for (auto&& o : Instances)
    {
        if (o.second->GetType() == type) ++c;
    }

    return c;
}

RHANDLE RObject::RegisterNewObject(RObject* pObject)
{
    const RHANDLE r = IssueNewObjectHandle();
    Instances[r] = pObject;
    return r;
}

RHANDLE RObject::FindHandle(const char* pszName)
{
    for (auto&& o : Instances)
    {
        if (_stricmp(o.second->GetStringName(), pszName) == 0)
            return o.first;
    }

    return RHANDLE_INVALID;
}

size_t RObject::UnloadAll()
{
    for (auto&& o : Instances)
    {
        delete o.second;
    }

    size_t c = Instances.size();
    Instances.clear();
    return c;
}

RObject* RObject::FindRObject(RHANDLE r)
{
    auto it = Instances.find(r);
    if (it != Instances.end())
        return it->second;

    return nullptr;
}
