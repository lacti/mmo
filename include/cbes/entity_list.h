#ifndef __ENTITY_LIST_H__
#define __ENTITY_LIST_H__

#include "entity_def.h"
#include "entity.h"

class entity_list_t {
public:
    typedef boost::function<bool (const entity_ref&)> _Pred;
    typedef boost::function<void (const entity_ref&)> _Func;
    
private:
    typedef std::vector<entity_ref> entity_vec_t;
    typedef entity_vec_t::iterator entity_vec_iterator_t;

public:
    size_t size() const;
    template <class _Attr> size_t size() const;
    bool empty() const;
    
    void for_each(_Func);
    entity_list_t find(_Pred);
    entity_ref find_first_of(_Pred);

    void erase(entity_id_t entity_id);
    
    template <class _Attr> void for_each(_Func);
    template <class _Attr> entity_list_t find();
    template <class _Attr> entity_list_t find(_Pred);
    template <class _Attr> entity_ref find_first_of();
    template <class _Attr> entity_ref find_first_of(_Pred);

    template <class _Attr> typename _Attr::list_type find_attribute();
    template <class _Attr> typename _Attr::pointer_type find_first_attribute_of();
    
    void push_back(entity_ref);
    entity_vec_t::iterator begin() { return entity_vec.begin(); }
    entity_vec_t::iterator end() { return entity_vec.end(); }
    entity_vec_t::const_iterator begin() const { return entity_vec.begin(); }
    entity_vec_t::const_iterator end() const { return entity_vec.end(); }

    entity_list_t() {}
    entity_list_t(const entity_list_t& other)
        : entity_vec(other.entity_vec) {}
    entity_list_t(entity_list_t&& move)
        : entity_vec(std::forward<entity_vec_t>(move.entity_vec)) {}

private:
    entity_vec_t entity_vec;
};


inline size_t entity_list_t::size() const
{
    return entity_vec.size();
}

template <class _Attr>
inline size_t entity_list_t::size() const
{
    return std::size_if(entity_vec.begin(), entity_vec.end(), [=] (entity_ref entity) {
        return entity->has<_Attr>();
    });
}

inline bool entity_list_t::empty() const
{
    return entity_vec.empty();
}


inline void entity_list_t::for_each(_Func func)
{
    std::for_each(entity_vec.begin(), entity_vec.end(), [&func] (entity_ref entity) {
        func(entity);
    });
}

inline entity_list_t entity_list_t::find(_Pred pred)
{
    entity_list_t entities;
    for_each([&entities, &pred] (entity_ref entity) {
        if (pred(entity))
            entities.push_back(entity);
    });
    return entities;
}

inline entity_ref entity_list_t::find_first_of(_Pred pred)
{
    entity_vec_iterator_t iter = std::find_if(entity_vec.begin(), entity_vec.end(), 
        [&pred] (entity_ref entity) {
            return pred(entity);
    });
    return iter != entity_vec.end()? (*iter): entity_ref();
}

inline void entity_list_t::erase(entity_id_t entity_id)
{
    for (auto it = entity_vec.begin(); it != entity_vec.end(); ) {
        if ((*it)->id == entity_id)
            it = entity_vec.erase(it);
        else ++it;
    }
}

template <class _Attr>
inline void entity_list_t::for_each(_Func func)
{
    for_each([&func] (entity_ref entity) {
        if (entity->has<_Attr>())
            func(entity);
    });
}

template <class _Attr>
inline entity_list_t entity_list_t::find()
{
    return find([=] (entity_ref entity) {
        return entity->has<_Attr>();
    });
}

template <class _Attr>
inline entity_list_t entity_list_t::find(_Pred pred)
{
    return find([&pred] (entity_ref entity) {
        return entity->has<_Attr>() && pred(entity);
    });
}

template <class _Attr>
inline entity_ref entity_list_t::find_first_of()
{
    return find_first_of([=] (entity_ref entity) {
        return entity->has<_Attr>();
    });
}

template <class _Attr>
inline entity_ref entity_list_t::find_first_of(_Pred pred)
{
    return find_first_of([&pred] (entity_ref entity) {
        return entity->has<_Attr>() && pred(entity);
    });
}

template <class _Attr>
inline typename _Attr::list_type entity_list_t::find_attribute()
{
    typename _Attr::list_type attributes;
    for_each([&attributes] (entity_ref entity) {
        if (entity->has<_Attr>())
            attributes.push_back(entity->get<_Attr>());
    });
    return attributes;
}

template <class _Attr>
inline typename _Attr::pointer_type entity_list_t::find_first_attribute_of()
{
    entity_ref entity = find_first_of<_Attr>();
    return entity != nullptr? entity->get<_Attr>(): typename _Attr::pointer_type();
}


inline void entity_list_t::push_back(entity_ref entity)
{
    entity_vec.push_back(entity);
}

#endif