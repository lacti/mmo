#pragma once
#include <data_def.h>
#include "data_type_id.h"
#include <data_center.h>

namespace data { ;
struct ps_t {
    data::id_t id;
    int count;
};

typedef data_center<ps_t> ps_data;

#if DATA_RELOADABLE
template <>
inline void data_referer<ps_t>::add()
{
    data_depend_map::instance().add(data::type_id::ps);
}

template <>
inline void data_referer<ps_t>::release()
{
    data_depend_map::instance().release(data::type_id::ps);
}

template <>
inline bool data_reloader_impl<ps_t>::is_reloadable()
{
    return !(data_depend_map::instance().is_depend(data::type_id::ps));
}

template <>
inline void data_reloader_impl<ps_t>::reload()
{
    data_center<ps_t>::clear();
    data_center<ps_t>::load();
    data_center<ps_t>::link();
}
#endif

void __data_load(data_type_t<ps_t>);

}
