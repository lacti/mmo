#include "stdafx.h"
#include "entity_manager.h"
#include "entity.h"
#include "component_name.h"
#include "component_pos0.h"
#include "component_pos1.h"
#include "component_sid.h"
#include "component_anim.h"

using namespace client;

__declspec(align(4)) LONG entity_manager::entity_id_issuer = 0;

IMPLEMENT_SINGLETON(entity_manager);

entity_manager::entity_manager()
    : player(0)
    , ecBusy(0)
{
}

entity entity_manager::new_entity(int id)
{
    //InterlockedIncrement(&entity_id_issuer);
    auto it = entities.find(id);
    if (it != entities.end())
    {
        it->second.id = id;
        return it->second;
    }

    return entities.insert(std::pair<int,entity>(id, entity(id))).first->second;
}

entity entity_manager::new_client_entity()
{
    InterlockedDecrement(&entity_id_issuer);

    return entities.insert(std::pair<int,entity>(entity_id_issuer, entity(entity_id_issuer))).first->second;
}
