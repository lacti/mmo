#pragma once

#include "component.h"

namespace component
{
    class garbage : public client::component_base
    {
    public:
        static const component_type::type component_index = component_type::GARBAGE;
        static tbb::concurrent_vector<garbage> data_array;
    };
}
