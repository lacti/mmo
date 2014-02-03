#pragma once
#include <data_def.h>
#include "data_type_id.h"
#include <data_center.h>

namespace data { ;
struct npc_name_t {
    data::id_t id;
    std::string name;
};

typedef data_center<npc_name_t> npc_name_data;

#if DATA_RELOADABLE
template <>
inline void data_referer<npc_name_t>::add()
{
    data_depend_map::instance().add(data::type_id::npc_name);
}

template <>
inline void data_referer<npc_name_t>::release()
{
    data_depend_map::instance().release(data::type_id::npc_name);
}

template <>
inline bool data_reloader_impl<npc_name_t>::is_reloadable()
{
    return !(data_depend_map::instance().is_depend(data::type_id::npc_name) 
        && data_depend_map::instance().is_depend(data::type_id::npc));
}

template <>
inline void data_reloader_impl<npc_name_t>::reload()
{
    data_center<npc_name_t>::clear();
    data_center<npc_name_t>::load();
    data_center<npc_name_t>::link();
}
#endif

void __data_load(data_type_t<npc_name_t>);

}
