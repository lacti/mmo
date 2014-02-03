#pragma once
#include <data_def.h>
#include "data_type_id.h"
#include <data_center.h>
#include "world_name_data.h"

namespace data { ;
struct world_t {
    data::id_t id;
    const world_name_t* world_name;
    struct world_npc_t {
        data::id_t id;
        const npc_t* npc;
        data::pos_t loc;
        double rot;
    };
    typedef std::map<data::id_t, world_npc_t*> world_npc_map_t;
    world_npc_map_t world_npc_map;
    
    ~world_t()
    {
        CLEAR_POINTER_MAP(world_npc_map_t, world_npc_map);
    }
};

typedef data_center<world_t> world_data;

#if DATA_RELOADABLE
template <>
inline void data_referer<world_t>::add()
{
    data_depend_map::instance().add(data::type_id::world);
}

template <>
inline void data_referer<world_t>::release()
{
    data_depend_map::instance().release(data::type_id::world);
}

template <>
inline bool data_reloader_impl<world_t>::is_reloadable()
{
    return !(data_depend_map::instance().is_depend(data::type_id::world));
}

template <>
inline void data_reloader_impl<world_t>::reload()
{
    data_center<world_t>::clear();
    data_center<world_t>::load();
    data_center<world_t>::link();
}
#endif

void __data_load(data_type_t<world_t>);

}
