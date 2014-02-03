/* this code is auto-generated. */

#pragma once

#include "cbes/attribute.h"

struct player_t : public attribute_t<player_t> {
    volatile bool logged;
    
    // default constructor
    player_t()
        : logged(false) {}
    
    // argumented constructor
    player_t(bool _logged)
        : logged(_logged) {}
    
    virtual void from_bson(bson_iterator*);
    virtual void to_bson(bson*);
    virtual void from_xml(TiXmlElement*);
    virtual void to_xml(std::ostream&);
};
typedef boost::shared_ptr<player_t> player_ref;
