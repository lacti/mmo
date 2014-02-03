/* this code is auto-generated. */

#pragma once

#include "cbes/attribute.h"

struct nameplate_t : public attribute_t<nameplate_t> {
    std::string title;
    std::string name;
    
    // default constructor
    nameplate_t()
        : title(), name() {}
    
    // argumented constructor
    nameplate_t(std::string _title, std::string _name)
        : title(_title), name(_name) {}
    
    virtual void from_bson(bson_iterator*);
    virtual void to_bson(bson*);
    virtual void from_xml(TiXmlElement*);
    virtual void to_xml(std::ostream&);
};
typedef boost::shared_ptr<nameplate_t> nameplate_ref;
