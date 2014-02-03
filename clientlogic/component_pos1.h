#pragma once

#include "component.h"

namespace component
{
    class pos1 : public client::component_base
    {
    public:
        pos1() : x(0), y(0), z(0) {}
        ~pos1() {}

        static const component_type::type component_index = component_type::POS1;
        static tbb::concurrent_vector<pos1> data_array;

        double x;
        double y;
		double z;
    };
}
