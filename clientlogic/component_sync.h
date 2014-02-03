#pragma once

#include "component.h"

namespace component
{
    class sync : public client::component_base
    {
    public:
        static const component_type::type component_index = component_type::SYNC;
        static tbb::concurrent_vector<sync> data_array;

        static const double XY_SYNC_PERIOD;

        bool force_sync;
        double last_xy_sync_time;
    };
}
