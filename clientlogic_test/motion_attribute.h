/* this code is auto-generated. */

#pragma once

#include "cbes/attribute.h"

struct motion_t : public attribute_t<motion_t> {
    double dir;
    double speed;
    
    // default constructor
    motion_t()
        : dir(0), speed(0) {}
    
    // argumented constructor
    motion_t(double _dir, double _speed)
        : dir(_dir), speed(_speed) {}
    
    virtual void from_bson(bson_iterator*);
    virtual void to_bson(bson*);
    virtual void from_xml(TiXmlElement*);
    virtual void to_xml(std::ostream&);
};
typedef boost::shared_ptr<motion_t> motion_ref;
