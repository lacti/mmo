#pragma once

namespace component_type
{
    enum type
    {
        SID,
        NAME,
        POS0,
        POS1,
        ANIM,
        SYNC,
        OWNER,
        GARBAGE,
        ENTITY_TEMPLATE,
		HP,
		SEQANIM,
        COLLISION,
        STATIC_ENTITY_TEMPLATE,

        MAX_NUM
    };
}

namespace client
{
    class component_base
    {
    public:
        component_base() : garbage(false)
        {
        }

        bool garbage;
    };
}

#include <tbb/concurrent_vector.h>
