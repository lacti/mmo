/* this code is auto-generated. */

#pragma once
#include <data_def.h>
#include "data_type_id.h"
#include <data_center.h>
#include "sprite_data.h"

namespace data { ;
struct character_t {
        data::id_t id;
        const sprite_t* sprite;
        
        ~character_t()
        {
        
        }
    };

typedef data_center<character_t> character_data;

#if DATA_RELOADABLE
template <>
inline void data_referer<character_t>::add()
{
    data_depend_map::instance().add(data::type_id::character);
}

template <>
inline void data_referer<character_t>::release()
{
    data_depend_map::instance().release(data::type_id::character);
}

template <>
inline bool data_reloader_impl<character_t>::is_reloadable()
{
    return !(data_depend_map::instance().is_depend(data::type_id::character));
}

template <>
inline void data_reloader_impl<character_t>::reload()
{
    data_center<character_t>::clear();
    data_center<character_t>::load();
    data_center<character_t>::link();
}
#endif

void __data_load(data_type_t<character_t>);

}
