/* this code is auto-generated. */

#pragma once
#include <data_def.h>
#include "data_type_id.h"
#include <data_center.h>

namespace data { ;
struct attribute_template_t {
        data::id_t id;
        std::string name;
        struct field_t {
                std::string name;
                std::string value;
                
                ~field_t()
                {
                
                }
            };
        typedef std::vector<field_t*> field_list_t;
        field_list_t field_list;
        
        ~attribute_template_t()
        {
            CLEAR_POINTER_LIST(field_t, field_list);
        }
    };

typedef data_center<attribute_template_t> attribute_template_data;

#if DATA_RELOADABLE
template <>
inline void data_referer<attribute_template_t>::add()
{
    data_depend_map::instance().add(data::type_id::attribute_template);
}

template <>
inline void data_referer<attribute_template_t>::release()
{
    data_depend_map::instance().release(data::type_id::attribute_template);
}

template <>
inline bool data_reloader_impl<attribute_template_t>::is_reloadable()
{
    return !(data_depend_map::instance().is_depend(data::type_id::attribute_template));
}

template <>
inline void data_reloader_impl<attribute_template_t>::reload()
{
    data_center<attribute_template_t>::clear();
    data_center<attribute_template_t>::load();
    data_center<attribute_template_t>::link();
}
#endif

void __data_load(data_type_t<attribute_template_t>);

}
