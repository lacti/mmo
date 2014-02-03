#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "entity_def.h"
#include "attribute.h"
#include "exclusive_run.h"

/*
    // use entity.
    entity_t ant;
    movable_ref mov = ant.attach<movable_t>();
    if (ant.has<movable_t>())
    {
        mov = ant.get<movable_t>();
        // do something
    }
*/

class entity_t {
public:
    typedef std::hash_map<attribute_key_t, attribute_ref> attribute_map_t;
    typedef attribute_map_t::iterator attribute_map_iterator_t;
    
    typedef std::function<void ()> work_t;
    typedef tbb::concurrent_queue<work_t> work_queue_t;

public:
    template <class _Attr> typename _Attr::pointer_type attach();
    template <class _Attr> bool detach();
    template <class _Attr> typename _Attr::pointer_type attach_or_get();
    template <class _Attr> typename _Attr::pointer_type get();
    template <class _Attr> typename _Attr::value_type copy();
    template <class _Attr> typename _Attr::const_pointer_type get() const;
    template <class _Attr> bool has() const;
    
    // for persistence
    attribute_ref add_attribute(attribute_ref attr);

    template <typename _Func>
    void enumerate_attributes(_Func&& func);

    // for async work
    void post(work_t work);

    entity_t();

    friend class entity_manager;

public: // property
    entity_id_t id;

private: // member variable
    attribute_map_t attribute_map;
    work_queue_t work_queue;

    exclusive_run_t::flag_t work_execlusion;
};


inline entity_t::entity_t()
    : id(INVALID_ENTITY_ID)
{
    exclusive_run_t::init(&work_execlusion);
}

template <class _Attr>
inline typename _Attr::pointer_type entity_t::attach()
{
    typename _Attr::pointer_type attribute_ptr(new _Attr);
    attribute_ptr->entity_id = id;

    std::pair<attribute_map_iterator_t, bool> result =
        attribute_map.insert(std::make_pair(_Attr::type_hash(), attribute_ptr));

    assert(result.second);
    return attribute_ptr;
}

template <class _Attr>
inline bool entity_t::detach()
{
    attribute_map_iterator_t iterator = attribute_map.find(_Attr::type_hash());
    if (iterator == attribute_map.end())
        return false;

    attribute_map.erase(iterator);
    return true;
}

template <class _Attr>
typename _Attr::pointer_type entity_t::attach_or_get()
{
    return has<_Attr>()? get<_Attr>(): attach<_Attr>();
}

template <class _Attr>
inline typename _Attr::pointer_type entity_t::get()
{
    attribute_map_iterator_t iterator = attribute_map.find(_Attr::type_hash());
    if (iterator == attribute_map.end())
        return typename _Attr::pointer_type();

    return typename _Attr::pointer_type(boost::shared_polymorphic_downcast<_Attr>(iterator->second));
}

template <class _Attr>
inline typename _Attr::value_type entity_t::copy()
{
    attribute_map_iterator_t iterator = attribute_map.find(_Attr::type_hash());
    if (iterator == attribute_map.end())
        return typename _Attr::value_type();

    return *(boost::shared_polymorphic_downcast<_Attr>(iterator->second));
}

template <class _Attr>
inline typename _Attr::const_pointer_type entity_t::get() const
{
    attribute_map_iterator_t iterator = attribute_map.find(_Attr::type_hash());
    if (iterator == attribute_map.end())
        return typename _Attr::pointer_type();

    return typename _Attr::pointer_type(boost::shared_polymorphic_downcast<_Attr>(iterator->second));
}

template <class _Attr>
inline bool entity_t::has() const
{
    return attribute_map.find(_Attr::type_hash()) != attribute_map.end();
}

inline attribute_ref entity_t::add_attribute(attribute_ref attribute_ptr)
{
    attribute_ptr->entity_id = id;
    
    attribute_key_t key = GET_ATTRIBUTE_KEY_FROM_TYPEINFO(*attribute_ptr);
    std::pair<attribute_map_iterator_t, bool> result =
        attribute_map.insert(std::make_pair(key, attribute_ptr));

    assert(result.second);
    return attribute_ptr;
}

template <typename _Func>
inline void entity_t::enumerate_attributes(_Func&& func)
{
    std::for_each(attribute_map.begin(), attribute_map.end(), [&] (std::pair<attribute_key_t, attribute_ref> pair) {
        func(pair.second);
    });
}

inline void entity_t::post(work_t work)
{
    work_queue.push(work);

    do {
        exclusive_run_t exec(&work_execlusion);
        if (!exec.is_acquired())
            return;
        
        // only-one thread do each work in work queue
        work_t each;
        while (work_queue.try_pop(each)) {
            each();
        }
    } while (!work_queue.empty());
}

#endif