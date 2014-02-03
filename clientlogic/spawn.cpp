#include "stdafx.h"
#include "clientlogic.h"
#include "clientlogic_private.h"
#include "entity.h"
#include "component_seqanim.h"
#include "component_anim.h"
#include "component_pos0.h"
#include "component_pos1.h"
#include "component_entity_template.h"
#include "component_collision.h"
#include "component_static_object_template.h"

CLIENTLOGIC_API int cl_spawn_entity()
{
    if (ClCtxEm() == nullptr)
        return CEC_CONTEXT_NOT_READY;

    client::entity e = ClCtxEm()->new_client_entity();

    ClCtxEm()->plug<component::seqanim>(e);
    ClCtxEm()->plug<component::anim>(e);
    ClCtxEm()->plug<component::pos0>(e);
    ClCtxEm()->plug<component::entity_template>(e);

    const float x = ((float)rand() / RAND_MAX) * 10 - 5;
    const float y = ((float)rand() / RAND_MAX) * 10 - 5;

    ClCtxEm()->get_as<component::pos0>(e).x = x;
    ClCtxEm()->get_as<component::pos0>(e).y = y;
    ClCtxEm()->get_as<component::entity_template>(e).resource_name = 8;

    return CEC_OKAY;
}

CLIENTLOGIC_API int cl_spawn_fireball(int owner_entity_id)
{
    if (ClCtxEm() == nullptr)
        return CEC_CONTEXT_NOT_READY;

    client::entity e = ClCtxEm()->new_client_entity();

    ClCtxEm()->plug<component::seqanim>(e);
    ClCtxEm()->plug<component::anim>(e);
    ClCtxEm()->plug<component::pos0>(e);
    ClCtxEm()->plug<component::pos1>(e);
    ClCtxEm()->plug<component::entity_template>(e);

    const double x = ClCtxEm()->get_as<component::pos0>(client::entity(owner_entity_id)).x;
    const double y = ClCtxEm()->get_as<component::pos0>(client::entity(owner_entity_id)).y;

    ClCtxEm()->get_as<component::pos0>(e).x = x;
    ClCtxEm()->get_as<component::pos0>(e).y = y;
    ClCtxEm()->get_as<component::entity_template>(e).resource_name = 8;

    const static int moveDistance = 100;
    int dx = 0, dy = 0;
    switch (ClCtxEm()->get_as<component::anim>(client::entity(owner_entity_id)).cur_dir)
    {
    case EMove::LEFT:
        dx = -moveDistance;
        break;
    case EMove::RIGHT:
        dx = moveDistance;
        break;
    case EMove::UP:
        dy = moveDistance;
        break;
    case EMove::DOWN:
        dy = -moveDistance;
        break;
    }

    ClCtxEm()->get_as<component::pos1>(e).x = x + dx;
    ClCtxEm()->get_as<component::pos1>(e).y = y + dy;

    ClCtxEm()->get_as<component::anim>(e).move_speed = 50;

    return CEC_OKAY;
}

CLIENTLOGIC_API int cl_spawn_cylinder()
{
    if (ClCtxEm() == nullptr)
        return CEC_CONTEXT_NOT_READY;

    client::entity e = ClCtxEm()->new_client_entity();

    using namespace component;

    PLUG<pos0>(e);
    PLUG<entity_template>(e);
    PLUG<collision>(e);

    const float x = ((float)rand() / RAND_MAX) * 300 - 150;
    const float y = ((float)rand() / RAND_MAX) * 300 - 150;

    C<pos0>(e).x = x;
    C<pos0>(e).y = y;
    C<static_object_template>(e).resource_name = 1; // mmo-data.xml
    C<collision>(e).add_to_world(e);

    return CEC_OKAY;
}
