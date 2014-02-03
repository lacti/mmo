/* this code is auto-generated. */

#pragma once
#include <data_def.h>
#include "data_type_id.h"
#include <data_center.h>

namespace data { ;
struct config_t {
        data::id_t id;
        int winx;
        int winy;
    };

typedef data_center<config_t> config_data;

#if DATA_RELOADABLE
template <>
inline void data_referer<config_t>::add()
{
    data_depend_map::instance().add(data::type_id::config);
}

template <>
inline void data_referer<config_t>::release()
{
    data_depend_map::instance().release(data::type_id::config);
}

template <>
inline bool data_reloader_impl<config_t>::is_reloadable()
{
    return !(data_depend_map::instance().is_depend(data::type_id::config));
}

template <>
inline void data_reloader_impl<config_t>::reload()
{
    data_center<config_t>::clear();
    data_center<config_t>::load();
    data_center<config_t>::link();
}
#endif

void __data_load(data_type_t<config_t>);

}
