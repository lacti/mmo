#pragma once

#include "component.h"

namespace client {

    class entity {
    public:
        enum
        {
            INVALID_ENTITY_ID = 0
        };

		
        entity()
			:id(INVALID_ENTITY_ID)
		{
		}

        explicit entity(int in_id)
            : id(in_id)
        {
        }

        bool operator < (const entity& rhs) const { return id < rhs.id; }
        bool operator == (const entity& rhs) const { return is_valid() && rhs.is_valid() && id == rhs.id; }

        bool is_valid() const;
        int get_id() const { return id; }
        void invalidate();

        int id;
        // 여기에는 절대로 멤버 변수를 추가하면 안됨
    };

}
