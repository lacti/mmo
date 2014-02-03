#pragma once

#include "component.h"

namespace component
{
    class name : public client::component_base
    {
    public:
        static const component_type::type component_index = component_type::NAME;
        static tbb::concurrent_vector<name> data_array;

		name() : nameplate_visible(false) {}

        std::string n;
        bool nameplate_visible;
    };
}
