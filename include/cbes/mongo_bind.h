#ifndef __MONGO_BING_H__
#define __MONGO_BING_H__

#include "entity.h"
#include "entity_bind.h"
#include "attribute.h"

class mongo_bind : public entity_bind_interface
{
public:
    mongo_bind();
    virtual ~mongo_bind();

    bool init(const char* ip, uint16_t port, const char* table_name);

    virtual bool insert(entity_ref entity);
    virtual bool update(entity_ref entity);
    virtual bool remove(entity_ref entity);
    virtual bool list(boost::function<void (entity_ref)> functor);
    
private:
    void destroy();
    void entity_id_to_bson(entity_ref entity, bson* obj);
    void entity_to_bson(entity_ref entity, bson* obj);

private:
    mongo conn;
    mongo_write_concern write_concern;
    std::string table_name;
    
    bool initialized;
    bool destroyed;
};


inline mongo_bind::mongo_bind()
    : initialized(false), destroyed(false)
{
}

inline mongo_bind::~mongo_bind()
{
    destroy();
}

inline bool mongo_bind::init(const char* ip, uint16_t port, const char* _table_name)
{
    assert(!initialized);
    initialized = true;

    table_name = _table_name;

    const int DEFAULT_TIMEOUT = 30000;
    mongo_init(&conn);
    
    mongo_write_concern_init(&write_concern);
    write_concern.w = 1;
    mongo_write_concern_finish(&write_concern);

    mongo_set_op_timeout(&conn, DEFAULT_TIMEOUT);
    int status = mongo_client(&conn, ip, port);
    if (status == MONGO_OK)
        return true;

    return conn.err == MONGO_CONN_SUCCESS;
}

inline void mongo_bind::destroy()
{
    assert(initialized);
    assert(!destroyed);
    destroyed = false;

    mongo_destroy(&conn);
    mongo_write_concern_destroy(&write_concern);
}

inline bool mongo_bind::insert(entity_ref entity)
{
    assert(initialized);

    bson obj[1];
    entity_to_bson(entity, obj);

    bool success = (mongo_insert(&conn, table_name.c_str(), obj, &write_concern) == MONGO_OK);

    bson_destroy(obj);
    return success;
}

inline bool mongo_bind::update(entity_ref entity)
{
    assert(initialized);

    bson id[1], obj[1];
    entity_id_to_bson(entity, id);
    entity_to_bson(entity, obj);

    bool success = (mongo_update(&conn, table_name.c_str(), id, obj, MONGO_UPDATE_BASIC, &write_concern) == MONGO_OK);
    
    bson_destroy(obj);
    bson_destroy(id);
    return success;
}

inline bool mongo_bind::remove(entity_ref entity)
{
    assert(initialized);

    bson id[1];
    entity_id_to_bson(entity, id);

    bool success = (mongo_remove(&conn, table_name.c_str(), id, &write_concern) == MONGO_OK);

    bson_destroy(id);
    return success;
}

inline bool mongo_bind::list(boost::function<void (entity_ref)> functor)
{
    assert(initialized);

    bson query[1];
    mongo_cursor cursor[1];

    bson_init(query);
    bson_finish(query);

    mongo_cursor_init(cursor, &conn, table_name.c_str());
    mongo_cursor_set_query(cursor, query);

    while (mongo_cursor_next(cursor) == MONGO_OK) {
        const bson* obj = mongo_cursor_bson(cursor);
        bson_iterator iterator[1];
        bson_iterator_init(iterator, obj);

        entity_id_t entity_id;
        std::vector<attribute_ref> attributes;
        
        while (bson_iterator_next(iterator)) {
            const char* key = bson_iterator_key(iterator);
            if (stricmp(key, "_id") == 0)
                continue;
            else if (stricmp(key, ENTITY_ID_FIELD_NAME) == 0)
                entity_id = bson_iterator_int(iterator);
            else {
                // ignore change or mismatch class-name
                std::string attribute_name(key);
                if (!sys<attribute_factory>().exists(attribute_name))
                    continue;

                attribute_ref attribute = sys<attribute_factory>().create(attribute_name);
                bson_iterator sub_iterator[1];
                bson_iterator_from_buffer(sub_iterator, bson_iterator_value(iterator));
                attribute->from_bson(sub_iterator);

                attributes.push_back(attribute);
            }
        }

        entity_ref entity(new entity_t);
        entity->id = entity_id;
        std::for_each(attributes.begin(), attributes.end(), [&] (const attribute_ref& attribute) {
            entity->add_attribute(attribute);
        });
        functor(entity);
    }

    bson_destroy(query);
    mongo_cursor_destroy(cursor);
    return true;
}

inline void mongo_bind::entity_id_to_bson(entity_ref entity, bson* obj)
{
    bson_init(obj);
        bson_append_int(obj, ENTITY_ID_FIELD_NAME, entity->id);
    bson_finish(obj);
}

inline void mongo_bind::entity_to_bson(entity_ref entity, bson* obj)
{
    bson_init(obj);
        bson_append_int(obj, ENTITY_ID_FIELD_NAME, entity->id);
        entity->enumerate_attributes([&] (attribute_ref attribute) {
            attribute->to_bson(obj);
        });
    bson_finish(obj);
}

#endif
