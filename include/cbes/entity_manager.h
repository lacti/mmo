#ifndef __ENTITY_MANAGER_H__
#define __ENTITY_MANAGER_H__

#include "entity.h"
#include "entity_list.h"
#include "attribute.h"
#include "singleton.h"

class entity_manager : private boost::noncopyable {
public:
    entity_ref new_entity(bool do_insert = true);
    entity_ref new_entity(entity_id_t id, bool do_insert = true);
    entity_id_t issue_entity_id();
    bool update_entity_id(entity_id_t old_id, entity_id_t new_id);

    bool insert(entity_ref entity);
    bool erase(entity_id_t entity_id);
    bool contains(entity_id_t entity_id);
    entity_ref get(entity_id_t entity_id);
    entity_list_t get(entity_id_list_t id_list);

    void set_id_serial(boost::uint32_t new_serial);
    
protected:
    entity_manager()
        : entity_id_serial(ENTITY_ID_START) {}

private:
    typedef tbb::concurrent_hash_map<entity_id_t, entity_ref> entity_map_t;
    entity_map_t entity_map;

    volatile boost::uint32_t entity_id_serial;
};


inline entity_ref entity_manager::new_entity(bool do_insert)
{
    return new_entity(issue_entity_id(), do_insert);
}

inline entity_ref entity_manager::new_entity(entity_id_t id, bool do_insert)
{
    entity_ref entity(new entity_t);
    entity->id = id;

    if (do_insert)
        insert(entity);
    
    return entity;
}

inline entity_id_t entity_manager::issue_entity_id()
{
    return static_cast<entity_id_t>(boost::interprocess::ipcdetail::atomic_inc32(&entity_id_serial));
}

inline bool entity_manager::update_entity_id(entity_id_t old_id, entity_id_t new_id)
{
    if (!contains(old_id) || contains(new_id))
        return false;

    entity_ref entity = get(old_id);
    entity->id = new_id;
    if (!insert(entity)) {
        entity->id = old_id;
        return false;
    }
    erase(old_id);

    std::for_each(entity->attribute_map.begin(), entity->attribute_map.end(), [=] (entity_t::attribute_map_t::value_type pair) {
        pair.second->entity_id = new_id;
    });
    return true;
}

inline bool entity_manager::insert(entity_ref entity)
{
    entity_map_t::accessor entity_accessor;
    if (!entity_map.insert(entity_accessor, entity->id))
        return false;
    
    entity_accessor->second = entity;
    entity_accessor.release();
    return true;
}

inline bool entity_manager::erase(entity_id_t entity_id)
{
    entity_map_t::accessor entity_accessor;
    if (!entity_map.find(entity_accessor, entity_id))
        return false;

    entity_map.erase(entity_accessor);
    entity_accessor.release();
    return true;
}

inline bool entity_manager::contains(entity_id_t entity_id)
{
    entity_map_t::accessor entity_accessor;
    if (!entity_map.find(entity_accessor, entity_id))
        return false;

    entity_accessor.release();
    return true;
}

inline entity_ref entity_manager::get(entity_id_t entity_id)
{
    if (entity_id == INVALID_ENTITY_ID)
        return entity_ref();

    entity_map_t::accessor entity_accessor;
    if (!entity_map.find(entity_accessor, entity_id))
        return entity_ref();

    entity_ref entity = entity_accessor->second;
    entity_accessor.release();
    return entity;
}

inline entity_list_t entity_manager::get(entity_id_list_t id_list)
{
    entity_list_t result;
    for (entity_id_list_iterator_t iter = id_list.begin(); iter != id_list.end(); ++iter) {
        if (entity_ref entity = get(*iter))
            result.push_back(entity);
    }
    return result;
}

inline void entity_manager::set_id_serial(boost::uint32_t new_serial)
{
    assert(entity_id_serial == ENTITY_ID_START);
    entity_id_serial = new_serial;
}

#endif