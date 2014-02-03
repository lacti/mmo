#pragma once

#include "component.h"

namespace component
{
    class hp : public client::component_base
    {
    public:
        hp() : cur_hp(1), max_hp(1) {}
        ~hp() {}

        static const component_type::type component_index = component_type::HP;
        static tbb::concurrent_vector<hp> data_array;

        int cur_hp;
		int max_hp;
    };
}
