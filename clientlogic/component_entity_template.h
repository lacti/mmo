#pragma once

#include "component.h"

namespace component
{
    class entity_template : public client::component_base
    {
    public:
        entity_template() : resource_name(5) {}
        ~entity_template() {}

        static const component_type::type component_index = component_type::ENTITY_TEMPLATE;
        static tbb::concurrent_vector<entity_template> data_array;

        int resource_name;
    };
}
