#pragma once

#include "component.h"

namespace component
{
    class pos0 : public client::component_base
    {
    public:
        pos0() : x(0), y(0), z(0), dx(0), dy(0), dz(0) {}
        ~pos0() {}

        static const component_type::type component_index = component_type::POS0;
        static tbb::concurrent_vector<pos0> data_array;

        double x, y, z;
        double dx, dy, dz;
    };
}
