/* this code is auto-generated. */

#pragma once
#include <data_def.h>
#include "data_type_id.h"
#include <data_center.h>

namespace data { ;
struct interact_template_t {
        enum id_t {
            ATTACK,
            HEAL,
            RECOVERY,
        };
        id_t id;
        struct effect_t {
                enum type_t {
                    ATTRIBUTE,
                };
                type_t type;
                enum direction_t {
                    TARGET,
                    OWNER,
                };
                direction_t direction;
                std::string attribute;
                std::string field;
                enum function_t {
                    MINUS,
                    PLUS,
                };
                function_t function;
                int value;
                
                ~effect_t()
                {
                
                }
            };
        typedef std::vector<effect_t*> effect_list_t;
        effect_list_t effect_list;
        
        ~interact_template_t()
        {
            CLEAR_POINTER_LIST(effect_t, effect_list);
        }
    };

typedef data_center<interact_template_t> interact_template_data;

#if DATA_RELOADABLE
template <>
inline void data_referer<interact_template_t>::add()
{
    data_depend_map::instance().add(data::type_id::interact_template);
}

template <>
inline void data_referer<interact_template_t>::release()
{
    data_depend_map::instance().release(data::type_id::interact_template);
}

template <>
inline bool data_reloader_impl<interact_template_t>::is_reloadable()
{
    return !(data_depend_map::instance().is_depend(data::type_id::interact_template));
}

template <>
inline void data_reloader_impl<interact_template_t>::reload()
{
    data_center<interact_template_t>::clear();
    data_center<interact_template_t>::load();
    data_center<interact_template_t>::link();
}
#endif

void __data_load(data_type_t<interact_template_t>);

}
