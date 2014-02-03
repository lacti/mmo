#pragma once

#include "component.h"

namespace component
{
    class owner : public client::component_base
    {
    public:
        static const component_type::type component_index = component_type::OWNER;
        static tbb::concurrent_vector<owner> data_array;

        bool own;
    };
}
