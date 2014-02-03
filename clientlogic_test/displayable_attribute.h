/* this code is auto-generated. */

#pragma once

#include "cbes/attribute.h"

struct displayable_t : public attribute_t<displayable_t> {
    bool visible;
    int resource_id;
    
    // default constructor
    displayable_t()
        : visible(true), resource_id(0) {}
    
    // argumented constructor
    displayable_t(int _resource_id)
        : visible(true), resource_id(_resource_id) {}
    
    virtual void from_bson(bson_iterator*);
    virtual void to_bson(bson*);
    virtual void from_xml(TiXmlElement*);
    virtual void to_xml(std::ostream&);
};
typedef boost::shared_ptr<displayable_t> displayable_ref;
