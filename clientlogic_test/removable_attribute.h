/* this code is auto-generated. */

#pragma once

#include "cbes/attribute.h"

struct removable_t : public attribute_t<removable_t> {
    bool action;
    bool touch;
    
    // default constructor
    removable_t()
        : action(true), touch(true) {}
    
    virtual void from_bson(bson_iterator*);
    virtual void to_bson(bson*);
    virtual void from_xml(TiXmlElement*);
    virtual void to_xml(std::ostream&);
};
typedef boost::shared_ptr<removable_t> removable_ref;
