#ifndef __ENTITY_DEF_H__
#define __ENTITY_DEF_H__

#define INVALID_ENTITY_ID           (static_cast<entity_id_t>(-1))
#define ENTITY_ID_START             (static_cast<entity_id_t>(1))
#define ENTITY_ID_FIELD_NAME        "entity_id"

// entity id (+ persistence object, - volatile object)
typedef int32_t entity_id_t;
typedef std::vector<entity_id_t> entity_id_list_t;
typedef entity_id_list_t::iterator entity_id_list_iterator_t;

class attribute_base_t;

typedef boost::shared_ptr<attribute_base_t> attribute_ref;
typedef std::vector<attribute_ref> attribute_list_t;
typedef attribute_list_t::iterator attribute_list_iterator_t;

typedef std::size_t attribute_key_t;
#define GET_ATTRIBUTE_KEY_FROM_TYPEINFO(typeval)    \
    typeid(typeval).hash_code()

class entity_t;

typedef boost::shared_ptr<entity_t> entity_ref;

#endif