/* this code is auto-generated. */

#pragma once
#include <data_def.h>
#include "data_type_id.h"
#include <data_center.h>

namespace data { ;
struct anim_state_t {
        data::id_t id;
        struct state_t {
                std::string name;
                
                ~state_t()
                {
                
                }
            };
        typedef std::vector<state_t*> state_list_t;
        state_list_t state_list;
    };

typedef data_center<anim_state_t> anim_state_data;

#if DATA_RELOADABLE
template <>
inline void data_referer<anim_state_t>::add()
{
    data_depend_map::instance().add(data::type_id::anim_state);
}

template <>
inline void data_referer<anim_state_t>::release()
{
    data_depend_map::instance().release(data::type_id::anim_state);
}

template <>
inline bool data_reloader_impl<anim_state_t>::is_reloadable()
{
    return !(data_depend_map::instance().is_depend(data::type_id::anim_state));
}

template <>
inline void data_reloader_impl<anim_state_t>::reload()
{
    data_center<anim_state_t>::clear();
    data_center<anim_state_t>::load();
    data_center<anim_state_t>::link();
}
#endif

void __data_load(data_type_t<anim_state_t>);

}
