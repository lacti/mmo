/* this code is auto-generated. */

#pragma once
#include <data_def.h>
#include "data_type_id.h"
#include <data_center.h>
#include "character_data.h"
#include "npc_name_data.h"

namespace data { ;
struct npc_t {
        data::id_t id;
        const character_t* character;
        const npc_name_t* npc_name;
        
        ~npc_t()
        {
        
        }
    };

typedef data_center<npc_t> npc_data;

#if DATA_RELOADABLE
template <>
inline void data_referer<npc_t>::add()
{
    data_depend_map::instance().add(data::type_id::npc);
}

template <>
inline void data_referer<npc_t>::release()
{
    data_depend_map::instance().release(data::type_id::npc);
}

template <>
inline bool data_reloader_impl<npc_t>::is_reloadable()
{
    return !(data_depend_map::instance().is_depend(data::type_id::npc));
}

template <>
inline void data_reloader_impl<npc_t>::reload()
{
    data_center<npc_t>::clear();
    data_center<npc_t>::load();
    data_center<npc_t>::link();
}
#endif

void __data_load(data_type_t<npc_t>);

}
