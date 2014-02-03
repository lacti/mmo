/* this code is auto-generated. */

#pragma once
#include <data_def.h>
#include "data_type_id.h"
#include <data_center.h>

namespace data { ;
struct static_object_t {
        data::id_t id;
        std::string name;
        std::string dds;
        double scale;
    };

typedef data_center<static_object_t> static_object_data;

#if DATA_RELOADABLE
template <>
inline void data_referer<static_object_t>::add()
{
    data_depend_map::instance().add(data::type_id::static_object);
}

template <>
inline void data_referer<static_object_t>::release()
{
    data_depend_map::instance().release(data::type_id::static_object);
}

template <>
inline bool data_reloader_impl<static_object_t>::is_reloadable()
{
    return !(data_depend_map::instance().is_depend(data::type_id::static_object));
}

template <>
inline void data_reloader_impl<static_object_t>::reload()
{
    data_center<static_object_t>::clear();
    data_center<static_object_t>::load();
    data_center<static_object_t>::link();
}
#endif

void __data_load(data_type_t<static_object_t>);

}
