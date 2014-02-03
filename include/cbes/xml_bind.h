#ifndef __XML_BIND_H__
#define __XML_BIND_H__

#include "entity.h"
#include "entity_bind.h"
#include "attribute.h"

class xml_bind : public entity_bind_interface
{
public:
    xml_bind();
    virtual ~xml_bind();
    
    bool load(const char* filename);
    bool save(const char* filename);
    
    virtual bool insert(entity_ref entity);
    virtual bool update(entity_ref entity);
    virtual bool remove(entity_ref entity);
    virtual bool list(boost::function<void (entity_ref)> functor);

private:
    typedef boost::interprocess::interprocess_recursive_mutex mutex_type;
    typedef boost::interprocess::scoped_lock<mutex_type> lock_type;

    mutex_type mutex;

    // lock based entity map
    typedef std::map<entity_id_t, entity_ref> entity_map_t;
    entity_map_t entity_map;
};


inline xml_bind::xml_bind()
{
}

inline xml_bind::~xml_bind()
{
}

inline bool xml_bind::load(const char* filename)
{
    lock_type lock(mutex);

#define XML_FOREACH(child, parent) \
    for (TiXmlElement* child = parent->FirstChildElement(); child != nullptr; child = child->NextSiblingElement())

    TiXmlDocument doc;
    if (!doc.LoadFile(filename))
        return false;

    TiXmlElement* root_node = doc.FirstChildElement("entity-list");
    XML_FOREACH(each_node, root_node) {
        entity_ref entity(new entity_t);
        each_node->Attribute(ENTITY_ID_FIELD_NAME, &entity->id);
        XML_FOREACH(attr_node, each_node) {
            const char* attr_name = attr_node->Value();
            if (!sys<attribute_factory>().exists(attr_name))
                continue;

            attribute_ref attribute = sys<attribute_factory>().create(attr_name);
            attribute->from_xml(attr_node);
            entity->add_attribute(attribute);
        }
        entity_map.insert(std::make_pair(entity->id, entity));
    }
    return true;
}

inline bool xml_bind::save(const char* filename)
{
    lock_type lock(mutex);

    std::ofstream out(filename, std::ios_base::out);
    out << "<?xml encoding=\"utf-8\" version=\"1.0\"?>" << std::endl;
    out << "<entity-list>" << std::endl;

    std::for_each(entity_map.begin(), entity_map.end(), [&out] (entity_map_t::value_type pair) {
        std::ostream& _out = out;
        entity_ref entity = pair.second;
        out << "<entity " << ENTITY_ID_FIELD_NAME << "=\"" << entity->id << "\">" << std::endl;
        entity->enumerate_attributes([&_out, &entity] (attribute_ref attribute) {
            attribute->to_xml(_out);
        });
        out << "</entity>" << std::endl;
    });

    out << "</entity-list>" << std::endl;
    return true;
}

inline bool xml_bind::insert(entity_ref entity)
{
    lock_type lock(mutex);

    auto ret = entity_map.insert(std::make_pair(entity->id, entity));
    return ret.second;
}

inline bool xml_bind::update(entity_ref entity)
{
    lock_type lock(mutex);

    remove(entity);
    return insert(entity);
}

inline bool xml_bind::remove(entity_ref entity)
{
    lock_type lock(mutex);

    auto it = entity_map.find(entity->id);
    if (it == entity_map.end())
        return false;

    entity_map.erase(entity->id);
    return true;
}

inline bool xml_bind::list(boost::function<void (entity_ref)> functor)
{
    lock_type lock(mutex);

    std::for_each(entity_map.begin(), entity_map.end(), [&functor] (entity_map_t::value_type pair) {
        functor(pair.second);
    });
    return true;
}
#endif
