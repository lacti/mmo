#pragma once

#include "component.h"

namespace component
{
    class seqanim : public client::component_base
    {
    public:
        static const component_type::type component_index = component_type::SEQANIM;
        static tbb::concurrent_vector<seqanim> data_array;

        seqanim()
            : frame_age(0)
			, frame_duration(0.08)
			, loop(false)
			, current_frame(-1)
        {}

		double frame_age;
        double frame_duration;
		bool loop;
		int current_frame;
    };
}
