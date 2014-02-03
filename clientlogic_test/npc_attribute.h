/* this code is auto-generated. */

#pragma once

#include "cbes/attribute.h"

struct npc_t : public attribute_t<npc_t> {
    bool interactive;
    
    // default constructor
    npc_t()
        : interactive(false) {}
    
    // argumented constructor
    npc_t(bool _interactive)
        : interactive(_interactive) {}
    
    virtual void from_bson(bson_iterator*);
    virtual void to_bson(bson*);
    virtual void from_xml(TiXmlElement*);
    virtual void to_xml(std::ostream&);
};
typedef boost::shared_ptr<npc_t> npc_ref;
