#pragma once

#include "component.h"

namespace component
{
    class sid : public client::component_base
    {
    public:
        sid() : v(0) {}

        static const component_type::type component_index = component_type::SID;
        static tbb::concurrent_vector<sid> data_array;
        
        int v;
    };
}
