/* this code is auto-generated. */

#pragma once

#include "cbes/attribute.h"

struct fightable_t : public attribute_t<fightable_t> {
    int max_hp;
    int hp;
    bool alive;
    
    // default constructor
    fightable_t()
        : max_hp(120), hp(100), alive(true) {}
    
    virtual void from_bson(bson_iterator*);
    virtual void to_bson(bson*);
    virtual void from_xml(TiXmlElement*);
    virtual void to_xml(std::ostream&);
};
typedef boost::shared_ptr<fightable_t> fightable_ref;
