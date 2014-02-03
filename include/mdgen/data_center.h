#pragma once
#include "data_def.h"
#include "data_linker.h"

#if DATA_RELOADABLE
#include "data_reloader.h"
#endif

namespace data { ;

template <typename _Ty>
class data_center {
public:
    typedef _Ty type;
    typedef type* pointer_type;
    typedef type& reference_type;
    typedef const type const_type;
    typedef const pointer_type const_pointer_type;
    typedef const reference_type const_reference_type;

public:
    static void reload();
    static bool add(pointer_type model);

public:
    pointer_type operator [] (data::id_t id);
    const_pointer_type operator [] (data::id_t id) const;

    template <typename _FuncTy>
    const_pointer_type find(_FuncTy) const;

    template <typename _FuncTy>
    void for_each(_FuncTy);

public:
    data_center();
    ~data_center();

#if DATA_RELOADABLE
    friend class data_reloader_impl<_Ty>;
#endif

private:
    static void clear();
    static void load();
    static void link();

public:
    static data_linker_t linker;

private:
    typedef std::map<data::id_t, pointer_type> data_map_t;

    struct storage_t {
        void clear();
        ~storage_t();
        
        data_map_t data_map;
    };
    static storage_t* storage;
};

template <typename _Ty>
inline void data_center<_Ty>::reload()
{
#if DATA_RELOADABLE
    data_reloader* reloader = new data_reloader_impl<_Ty>;
    if (reloader->is_reloadable())
        reloader->reload();

    else data_depend_map::instance().reloader_queue.push_back(reloader);
#endif
}

template <typename _Ty>
inline bool data_center<_Ty>::add(pointer_type model)
{
    if (storage == nullptr)
        storage = new storage_t;
    
    std::pair<data_map_t::iterator, bool> result = storage->data_map.insert(std::make_pair(model->id, model));
    return result.second;
}

template <typename _Ty>
inline void data_center<_Ty>::load()
{
    data::__data_load(data_type_t<_Ty>());
}

template <typename _Ty>
inline void data_center<_Ty>::clear()
{
    if (storage != nullptr)
        storage->clear();
    linker.clear();
}

template <typename _Ty>
inline void data_center<_Ty>::link()
{
    linker.link();
}

template <typename _Ty>
inline typename data_center<_Ty>::pointer_type data_center<_Ty>::operator [] (data::id_t id)
{
    if (storage->data_map.find(id) == storage->data_map.end())
        return nullptr;

    return storage->data_map[id];
}

template <typename _Ty>
inline typename data_center<_Ty>::const_pointer_type data_center<_Ty>::operator [] (data::id_t id) const
{
    if (storage->data_map.find(id) == storage->data_map.end())
        return nullptr;

    return storage->data_map[id];
}

template <typename _Ty>
template <typename _FuncTy>
inline typename data_center<_Ty>::const_pointer_type data_center<_Ty>::find(_FuncTy func) const
{
    auto it = std::find_if(storage->data_map.begin(), storage->data_map.end(), [&func] (data_map_t::value_type pair) {
        return func(pair.second);
    });
    if (it == storage->data_map.end())
        return nullptr;
    return it->second;
}

template <typename _Ty>
template <typename _FuncTy>
inline void data_center<_Ty>::for_each(_FuncTy func)
{
    std::for_each(storage->data_map.begin(), storage->data_map.end(), [&func] (data_map_t::value_type pair) {
        func(pair.second);
    });
}

template <typename _Ty>
inline data_center<_Ty>::data_center()
{
    if (storage == nullptr)
        storage = new storage_t;
    
#if DATA_RELOADABLE
    data_referer<_Ty> referer;
    referer.add();
#endif
}

template <typename _Ty>
inline data_center<_Ty>::~data_center()
{
#if DATA_RELOADABLE
    data_referer<_Ty> referer;
    referer.release();
#endif
}

template <typename _Ty>
inline void data_center<_Ty>::storage_t::clear()
{
    std::for_each(data_map.begin(), data_map.end(), [=] (data_map_t::value_type value) {
        delete value.second;
    });
    data_map.clear();
}

template <typename _Ty>
inline data_center<_Ty>::storage_t::~storage_t()
{
    clear();
}

}