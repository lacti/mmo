#ifndef __ENTITY_BIND_H_
#define __ENTITY_BIND_H_

#include "entity_def.h"

// bind prototype
class entity_bind_interface
{
public:
    virtual ~entity_bind_interface() {}

    virtual bool insert(entity_ref entity) { return false; }
    virtual bool update(entity_ref entity) { return false; }
    virtual bool remove(entity_ref entity) { return false; }
    virtual bool list(boost::function<void (entity_ref)> functor) { return false; }
};

// bind selector
class entity_bind : entity_bind_interface
{
public:
    entity_bind()
        : impl(nullptr) {}
    virtual ~entity_bind() {}

    virtual bool insert(entity_ref entity) { return impl->insert(entity); }
    virtual bool update(entity_ref entity) { return impl->update(entity); }
    virtual bool remove(entity_ref entity) { return impl->remove(entity); }
    virtual bool list(boost::function<void (entity_ref)> functor) { return impl->list(functor); }

    template <typename bind_sys>
    void set_impl();

    template <typename bind_sys>
    bool is();
    
private:
    entity_bind_interface* impl;
};

template <typename bind_sys>
inline void entity_bind::set_impl()
{
    impl = &sys<bind_sys>();
}

template <typename bind_sys>
inline bool entity_bind::is()
{
    return impl == &sys<bind_sys>();
}

#endif