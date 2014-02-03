#pragma once

#include "component.h"

namespace component
{
    class static_object_template : public client::component_base
    {
    public:
        static_object_template() : resource_name(0) {}
        ~static_object_template() {}

        static const component_type::type component_index = component_type::STATIC_ENTITY_TEMPLATE;
        static tbb::concurrent_vector<static_object_template> data_array;

        int resource_name;
    };
}
