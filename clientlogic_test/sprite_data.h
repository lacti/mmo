/* this code is auto-generated. */

#pragma once
#include <data_def.h>
#include "data_type_id.h"
#include <data_center.h>
#include "anim_state_data.h"

namespace data { ;
struct sprite_t {
        data::id_t id;
        std::string name;
        const anim_state_t* anim_state;
        int umax;
        int vmax;
        double scale;
        int count;
        int tile_size;
        
        ~sprite_t()
        {
        
        }
    };

typedef data_center<sprite_t> sprite_data;

#if DATA_RELOADABLE
template <>
inline void data_referer<sprite_t>::add()
{
    data_depend_map::instance().add(data::type_id::sprite);
}

template <>
inline void data_referer<sprite_t>::release()
{
    data_depend_map::instance().release(data::type_id::sprite);
}

template <>
inline bool data_reloader_impl<sprite_t>::is_reloadable()
{
    return !(data_depend_map::instance().is_depend(data::type_id::sprite));
}

template <>
inline void data_reloader_impl<sprite_t>::reload()
{
    data_center<sprite_t>::clear();
    data_center<sprite_t>::load();
    data_center<sprite_t>::link();
}
#endif

void __data_load(data_type_t<sprite_t>);

}
