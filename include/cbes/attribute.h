#ifndef __ATTRIBUTE_H__
#define __ATTRIBUTE_H__

#include "entity_def.h"

/*
    // use attribute.
    class my_attribute : public attribute_t<my_attribute>
    {
        // write your attribute code
    }
*/

// for vfptr
class attribute_base_t {
public:
    attribute_base_t()
        : entity_id(INVALID_ENTITY_ID) {}
    virtual ~attribute_base_t() {}

    #pragma region /* bind */
    virtual void from_bson(bson_iterator*)  {}
    virtual void to_bson(bson*)             {}
    virtual void from_xml(TiXmlElement*)    {}
    virtual void to_xml(std::ostream&)      {}
    #pragma endregion

public: // property
    entity_id_t entity_id;
};


// for type_traits & key
template <class _Attr>
class attribute_t : public attribute_base_t {
public:
    typedef _Attr value_type;
    typedef boost::shared_ptr<value_type> pointer_type;
    typedef const pointer_type const_pointer_type;
    typedef std::vector<pointer_type> list_type;

    static attribute_key_t type_hash();
};

template <class _Attr>
inline attribute_key_t attribute_t<_Attr>::type_hash()
{
    return GET_ATTRIBUTE_KEY_FROM_TYPEINFO(_Attr);
}


// attribute factory
class attribute_factory
{
public:
    bool exists(std::string attribute_name) const;
    attribute_ref create(std::string attribute_name);

public:
    //typedef attribute_ref (*factory_function) ();
    typedef boost::function<attribute_ref()> factory_function;
    typedef std::map<std::string, factory_function> factory_map_t;
    factory_map_t factory_map;
};

inline bool attribute_factory::exists(std::string attribute_name) const
{
    return factory_map.find(attribute_name) != factory_map.cend();
}

inline attribute_ref attribute_factory::create(std::string attribute_name)
{
    auto it = factory_map.find(attribute_name);
    assert(it != factory_map.end());
    return it->second();
}

#endif