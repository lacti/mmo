#pragma once
#include "component.h"
#include "entity.h"
#include <BulletCollision/CollisionDispatch/btCollisionObject.h>

namespace component
{
    class collision : public client::component_base
    {
    public:
        enum collision_type
        {
            box,
            capsule,
            cylinder
        };

        collision();
        ~collision();

        static const component_type::type component_index = component_type::COLLISION;
        static tbb::concurrent_vector<collision> data_array;

        void add_to_world(client::entity e);
        void remove_from_world();

        client::entity owner;
        collision_type type;
        double x, y, z;
        btCollisionObject bt_obj;
    };
}
