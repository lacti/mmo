/* this code is auto-generated. */

#pragma once

#include "cbes/attribute.h"

struct pos_t : public attribute_t<pos_t> {
    double x;
    double y;
    double z;
    
    // default constructor
    pos_t()
        : x(0), y(0), z(0) {}
    
    // argumented constructor
    pos_t(double _x, double _y, double _z)
        : x(_x), y(_y), z(_z) {}
    
    virtual void from_bson(bson_iterator*);
    virtual void to_bson(bson*);
    virtual void from_xml(TiXmlElement*);
    virtual void to_xml(std::ostream&);
    
    void copy_from(const pos_t& other)
    {
        x = other.x;
        y = other.y;
        z = other.z;
    }
    static double distance(pos_t p0, pos_t p1)
    {
        return sqrt(pow(p0.x - p1.x, 2) + pow(p0.y - p1.y, 2) + pow(p0.z - p1.z, 2));
    }
};
typedef boost::shared_ptr<pos_t> pos_ref;
