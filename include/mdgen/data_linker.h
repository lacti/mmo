#pragma once
#include <vector>
#include "data_def.h"

namespace data { ;

class data_linker_t
{
public:
    template <typename _ModelTy>
    void add(const _ModelTy *const * address, id_t id);

    void link();
    void clear();

private:
    struct linker_t
    {
        virtual void link() = 0;
        virtual ~linker_t() {}
    };

    typedef std::vector<linker_t*> linker_list_t;
    linker_list_t linker_list;
};

template <typename _ModelTy>
inline void data_linker_t::add(const _ModelTy *const * address, id_t id)
{
    typedef const _ModelTy *const * const_t;
    struct _linker_t : linker_t {
        typedef const _ModelTy** addr_t;

        virtual void link()
        {
            data_center<_ModelTy> data_accessor;
            *address = data_accessor[id];
        }

        _linker_t(const_t _address, id_t _id)
            : address(const_cast<addr_t>(_address)), id(_id) {}

        addr_t address;
        id_t id;
    };
    linker_t* lnk = new _linker_t(address, id);
    linker_list.push_back(lnk);
}

inline void data_linker_t::link()
{
    std::for_each(linker_list.begin(), linker_list.end(), [=] (linker_t* linker) {
        linker->link();
    });
}

inline void data_linker_t::clear()
{
    std::for_each(linker_list.begin(), linker_list.end(), [=] (linker_t* linker) {
        delete linker;
    });
    linker_list.clear();
}

}