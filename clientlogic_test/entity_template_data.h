/* this code is auto-generated. */

#pragma once
#include <data_def.h>
#include "data_type_id.h"
#include <data_center.h>

namespace data { ;
struct entity_template_t {
        enum id_t {
            SYSTEM,
            USER,
            NPC,
            ITEM,
            MONSTER,
            FACTORY,
        };
        id_t id;
        struct attribute_t {
                std::string name;
                
                ~attribute_t()
                {
                
                }
            };
        typedef std::vector<attribute_t*> attribute_list_t;
        attribute_list_t attribute_list;
        
        ~entity_template_t()
        {
            CLEAR_POINTER_LIST(attribute_t, attribute_list);
        }
    };

typedef data_center<entity_template_t> entity_template_data;

#if DATA_RELOADABLE
template <>
inline void data_referer<entity_template_t>::add()
{
    data_depend_map::instance().add(data::type_id::entity_template);
}

template <>
inline void data_referer<entity_template_t>::release()
{
    data_depend_map::instance().release(data::type_id::entity_template);
}

template <>
inline bool data_reloader_impl<entity_template_t>::is_reloadable()
{
    return !(data_depend_map::instance().is_depend(data::type_id::entity_template));
}

template <>
inline void data_reloader_impl<entity_template_t>::reload()
{
    data_center<entity_template_t>::clear();
    data_center<entity_template_t>::load();
    data_center<entity_template_t>::link();
}
#endif

void __data_load(data_type_t<entity_template_t>);

}
