/* this code is auto-generated. */

#pragma once

#include <msg_def.h>
#include <msg_reader.h>
#include <msg_writer.h>
#include <msg_session_ref.h>

#pragma warning (disable: 4100)

namespace msg_type {
    const msg_type_t enter_world = 1000;
    const msg_type_t spawn = 1001;
    const msg_type_t move = 1002;
    const msg_type_t update_position = 1003;
    const msg_type_t despawn = 1004;
    const msg_type_t world_info = 1005;
    const msg_type_t chat = 1006;
    const msg_type_t character_resource = 1007;
    const msg_type_t interact = 1008;
    const msg_type_t update_hp = 1009;
    const msg_type_t alert = 1010;
}


namespace msg {

struct enter_world_msg {
    static const msg_type_t __type = msg_type::enter_world;
    
    std::string name;
    
    enter_world_msg(std::string _name)
        : name(_name) {}
    
    #ifdef _DEBUG
    enter_world_msg()
        : name() {}
    #else
    enter_world_msg() {}
    #endif
    
    typedef enter_world_msg type;
    typedef std::function<void (msg_session_ref, const type&)> handler_t;
    typedef void (*listener_t)(const type&);
    typedef std::vector<listener_t> listener_list;
    
    static handler_t _handler;
    static listener_list _listeners;
    static void handle(msg_session_ref session, const type& msg);
    static void handle(msg_session_ref session, msg_reader reader);
    static void broadcast(const type& msg);
};

struct move_msg {
    static const msg_type_t __type = msg_type::move;
    
    int id;
    double x;
    double y;
    double dir;
    double speed;
    
    move_msg(int _id, double _x, double _y, double _dir, double _speed)
        : id(_id), x(_x), y(_y), dir(_dir), speed(_speed) {}
    
    #ifdef _DEBUG
    move_msg()
        : id(0), x(0), y(0), dir(0), speed(0) {}
    #else
    move_msg() {}
    #endif
    
    typedef move_msg type;
    typedef std::function<void (msg_session_ref, const type&)> handler_t;
    typedef void (*listener_t)(const type&);
    typedef std::vector<listener_t> listener_list;
    
    static handler_t _handler;
    static listener_list _listeners;
    static void handle(msg_session_ref session, const type& msg);
    static void handle(msg_session_ref session, msg_reader reader);
    static void broadcast(const type& msg);
};

struct update_position_msg {
    static const msg_type_t __type = msg_type::update_position;
    
    int id;
    double x;
    double y;
    double dir;
    double speed;
    bool instance_move;
    
    update_position_msg(int _id, double _x, double _y, double _dir, double _speed, bool _instance_move)
        : id(_id), x(_x), y(_y), dir(_dir), speed(_speed), instance_move(_instance_move) {}
    
    #ifdef _DEBUG
    update_position_msg()
        : id(0), x(0), y(0), dir(0), speed(0), instance_move(false) {}
    #else
    update_position_msg() {}
    #endif
    
    typedef update_position_msg type;
    typedef std::function<void (msg_session_ref, const type&)> handler_t;
    typedef void (*listener_t)(const type&);
    typedef std::vector<listener_t> listener_list;
    
    static handler_t _handler;
    static listener_list _listeners;
    static void handle(msg_session_ref session, const type& msg);
    static void handle(msg_session_ref session, msg_reader reader);
    static void broadcast(const type& msg);
};

struct despawn_msg {
    static const msg_type_t __type = msg_type::despawn;
    
    int id;
    
    despawn_msg(int _id)
        : id(_id) {}
    
    #ifdef _DEBUG
    despawn_msg()
        : id(0) {}
    #else
    despawn_msg() {}
    #endif
    
    typedef despawn_msg type;
    typedef std::function<void (msg_session_ref, const type&)> handler_t;
    typedef void (*listener_t)(const type&);
    typedef std::vector<listener_t> listener_list;
    
    static handler_t _handler;
    static listener_list _listeners;
    static void handle(msg_session_ref session, const type& msg);
    static void handle(msg_session_ref session, msg_reader reader);
    static void broadcast(const type& msg);
};

struct chat_msg {
    static const msg_type_t __type = msg_type::chat;
    
    int id;
    std::string message;
    
    chat_msg(int _id, std::string _message)
        : id(_id), message(_message) {}
    
    #ifdef _DEBUG
    chat_msg()
        : id(0), message() {}
    #else
    chat_msg() {}
    #endif
    
    typedef chat_msg type;
    typedef std::function<void (msg_session_ref, const type&)> handler_t;
    typedef void (*listener_t)(const type&);
    typedef std::vector<listener_t> listener_list;
    
    static handler_t _handler;
    static listener_list _listeners;
    static void handle(msg_session_ref session, const type& msg);
    static void handle(msg_session_ref session, msg_reader reader);
    static void broadcast(const type& msg);
};

struct character_resource_msg {
    static const msg_type_t __type = msg_type::character_resource;
    
    int id;
    int resource_id;
    
    character_resource_msg(int _id, int _resource_id)
        : id(_id), resource_id(_resource_id) {}
    
    #ifdef _DEBUG
    character_resource_msg()
        : id(0), resource_id(0) {}
    #else
    character_resource_msg() {}
    #endif
    
    typedef character_resource_msg type;
    typedef std::function<void (msg_session_ref, const type&)> handler_t;
    typedef void (*listener_t)(const type&);
    typedef std::vector<listener_t> listener_list;
    
    static handler_t _handler;
    static listener_list _listeners;
    static void handle(msg_session_ref session, const type& msg);
    static void handle(msg_session_ref session, msg_reader reader);
    static void broadcast(const type& msg);
};

struct interact_msg {
    static const msg_type_t __type = msg_type::interact;
    
    int interact_id;
    
    interact_msg(int _interact_id)
        : interact_id(_interact_id) {}
    
    #ifdef _DEBUG
    interact_msg()
        : interact_id(0) {}
    #else
    interact_msg() {}
    #endif
    
    typedef interact_msg type;
    typedef std::function<void (msg_session_ref, const type&)> handler_t;
    typedef void (*listener_t)(const type&);
    typedef std::vector<listener_t> listener_list;
    
    static handler_t _handler;
    static listener_list _listeners;
    static void handle(msg_session_ref session, const type& msg);
    static void handle(msg_session_ref session, msg_reader reader);
    static void broadcast(const type& msg);
};

struct update_hp_msg {
    static const msg_type_t __type = msg_type::update_hp;
    
    int id;
    int max_hp;
    int hp;
    
    update_hp_msg(int _id, int _max_hp, int _hp)
        : id(_id), max_hp(_max_hp), hp(_hp) {}
    
    #ifdef _DEBUG
    update_hp_msg()
        : id(0), max_hp(0), hp(0) {}
    #else
    update_hp_msg() {}
    #endif
    
    typedef update_hp_msg type;
    typedef std::function<void (msg_session_ref, const type&)> handler_t;
    typedef void (*listener_t)(const type&);
    typedef std::vector<listener_t> listener_list;
    
    static handler_t _handler;
    static listener_list _listeners;
    static void handle(msg_session_ref session, const type& msg);
    static void handle(msg_session_ref session, msg_reader reader);
    static void broadcast(const type& msg);
};

struct alert_msg {
    static const msg_type_t __type = msg_type::alert;
    
    std::string message;
    
    alert_msg(std::string _message)
        : message(_message) {}
    
    #ifdef _DEBUG
    alert_msg()
        : message() {}
    #else
    alert_msg() {}
    #endif
    
    typedef alert_msg type;
    typedef std::function<void (msg_session_ref, const type&)> handler_t;
    typedef void (*listener_t)(const type&);
    typedef std::vector<listener_t> listener_list;
    
    static handler_t _handler;
    static listener_list _listeners;
    static void handle(msg_session_ref session, const type& msg);
    static void handle(msg_session_ref session, msg_reader reader);
    static void broadcast(const type& msg);
};

struct spawn_msg {
    static const msg_type_t __type = msg_type::spawn;
    
    int id;
    std::string name;
    msg::character_resource_msg character_resource;
    msg::update_position_msg update_position;
    msg::update_hp_msg update_hp;
    
    spawn_msg(int _id, std::string _name)
        : id(_id), name(_name) {}
    
    #ifdef _DEBUG
    spawn_msg()
        : id(0), name() {}
    #else
    spawn_msg() {}
    #endif
    
    typedef spawn_msg type;
    typedef std::function<void (msg_session_ref, const type&)> handler_t;
    typedef void (*listener_t)(const type&);
    typedef std::vector<listener_t> listener_list;
    
    static handler_t _handler;
    static listener_list _listeners;
    static void handle(msg_session_ref session, const type& msg);
    static void handle(msg_session_ref session, msg_reader reader);
    static void broadcast(const type& msg);
};

struct world_info_msg {
    static const msg_type_t __type = msg_type::world_info;
    
    int id;
    int world_id;
    typedef std::vector<msg::spawn_msg> spawn_list;
    spawn_list spawns;
    
    world_info_msg(int _id, int _world_id)
        : id(_id), world_id(_world_id) {}
    
    #ifdef _DEBUG
    world_info_msg()
        : id(0), world_id(0) {}
    #else
    world_info_msg() {}
    #endif
    
    typedef world_info_msg type;
    typedef std::function<void (msg_session_ref, const type&)> handler_t;
    typedef void (*listener_t)(const type&);
    typedef std::vector<listener_t> listener_list;
    
    static handler_t _handler;
    static listener_list _listeners;
    static void handle(msg_session_ref session, const type& msg);
    static void handle(msg_session_ref session, msg_reader reader);
    static void broadcast(const type& msg);
};

}


#pragma region /* reader & writer */

template <>
inline msg_writer& msg_writer::operator << (const msg::enter_world_msg& msg)
{
    (*this) << msg_type::enter_world;
    (*this) << msg.name;
    return (*this);
}

template <>
inline msg_reader& msg_reader::operator >> (msg::enter_world_msg& msg)
{
    (*this) >> msg.name;
    return (*this);
}

template <>
inline msg_writer& msg_writer::operator << (const msg::move_msg& msg)
{
    (*this) << msg_type::move;
    (*this) << msg.id;
    (*this) << msg.x;
    (*this) << msg.y;
    (*this) << msg.dir;
    (*this) << msg.speed;
    return (*this);
}

template <>
inline msg_reader& msg_reader::operator >> (msg::move_msg& msg)
{
    (*this) >> msg.id;
    (*this) >> msg.x;
    (*this) >> msg.y;
    (*this) >> msg.dir;
    (*this) >> msg.speed;
    return (*this);
}

template <>
inline msg_writer& msg_writer::operator << (const msg::update_position_msg& msg)
{
    (*this) << msg_type::update_position;
    (*this) << msg.id;
    (*this) << msg.x;
    (*this) << msg.y;
    (*this) << msg.dir;
    (*this) << msg.speed;
    (*this) << msg.instance_move;
    return (*this);
}

template <>
inline msg_reader& msg_reader::operator >> (msg::update_position_msg& msg)
{
    (*this) >> msg.id;
    (*this) >> msg.x;
    (*this) >> msg.y;
    (*this) >> msg.dir;
    (*this) >> msg.speed;
    (*this) >> msg.instance_move;
    return (*this);
}

template <>
inline msg_writer& msg_writer::operator << (const msg::despawn_msg& msg)
{
    (*this) << msg_type::despawn;
    (*this) << msg.id;
    return (*this);
}

template <>
inline msg_reader& msg_reader::operator >> (msg::despawn_msg& msg)
{
    (*this) >> msg.id;
    return (*this);
}

template <>
inline msg_writer& msg_writer::operator << (const msg::chat_msg& msg)
{
    (*this) << msg_type::chat;
    (*this) << msg.id;
    (*this) << msg.message;
    return (*this);
}

template <>
inline msg_reader& msg_reader::operator >> (msg::chat_msg& msg)
{
    (*this) >> msg.id;
    (*this) >> msg.message;
    return (*this);
}

template <>
inline msg_writer& msg_writer::operator << (const msg::character_resource_msg& msg)
{
    (*this) << msg_type::character_resource;
    (*this) << msg.id;
    (*this) << msg.resource_id;
    return (*this);
}

template <>
inline msg_reader& msg_reader::operator >> (msg::character_resource_msg& msg)
{
    (*this) >> msg.id;
    (*this) >> msg.resource_id;
    return (*this);
}

template <>
inline msg_writer& msg_writer::operator << (const msg::interact_msg& msg)
{
    (*this) << msg_type::interact;
    (*this) << msg.interact_id;
    return (*this);
}

template <>
inline msg_reader& msg_reader::operator >> (msg::interact_msg& msg)
{
    (*this) >> msg.interact_id;
    return (*this);
}

template <>
inline msg_writer& msg_writer::operator << (const msg::update_hp_msg& msg)
{
    (*this) << msg_type::update_hp;
    (*this) << msg.id;
    (*this) << msg.max_hp;
    (*this) << msg.hp;
    return (*this);
}

template <>
inline msg_reader& msg_reader::operator >> (msg::update_hp_msg& msg)
{
    (*this) >> msg.id;
    (*this) >> msg.max_hp;
    (*this) >> msg.hp;
    return (*this);
}

template <>
inline msg_writer& msg_writer::operator << (const msg::alert_msg& msg)
{
    (*this) << msg_type::alert;
    (*this) << msg.message;
    return (*this);
}

template <>
inline msg_reader& msg_reader::operator >> (msg::alert_msg& msg)
{
    (*this) >> msg.message;
    return (*this);
}

template <>
inline msg_writer& msg_writer::operator << (const msg::spawn_msg& msg)
{
    (*this) << msg_type::spawn;
    (*this) << msg.id;
    (*this) << msg.name;
    (*this) << msg.character_resource;
    (*this) << msg.update_position;
    (*this) << msg.update_hp;
    return (*this);
}

template <>
inline msg_reader& msg_reader::operator >> (msg::spawn_msg& msg)
{
    (*this) >> msg.id;
    (*this) >> msg.name;
    this->skip(static_cast<msg_size_t>(sizeof(msg_type_t)));
    (*this) >> msg.character_resource;
    this->skip(static_cast<msg_size_t>(sizeof(msg_type_t)));
    (*this) >> msg.update_position;
    this->skip(static_cast<msg_size_t>(sizeof(msg_type_t)));
    (*this) >> msg.update_hp;
    return (*this);
}

template <>
inline msg_writer& msg_writer::operator << (const msg::world_info_msg& msg)
{
    (*this) << msg_type::world_info;
    (*this) << msg.id;
    (*this) << msg.world_id;
    (*this) << static_cast<uint32_t>(msg.spawns.size());
    for (msg::world_info_msg::spawn_list::const_iterator iter = msg.spawns.begin(); iter != msg.spawns.end(); ++iter) {
        (*this) << (*iter);
    }
    return (*this);
}

template <>
inline msg_reader& msg_reader::operator >> (msg::world_info_msg& msg)
{
    (*this) >> msg.id;
    (*this) >> msg.world_id;
    uint32_t spawn_count;
    (*this) >> spawn_count;
    for (uint32_t index = 0; index < spawn_count; ++index) {
        msg::spawn_msg each;
        this->skip(static_cast<msg_size_t>(sizeof(msg_type_t)));
        (*this) >> each;
        msg.spawns.push_back(each);
    }
    return (*this);
}

#pragma endregion


namespace msg {
inline void msg::enter_world_msg::handle(msg_session_ref session, const type& msg)
{
    _ASSERT(_handler != NULL);
    _handler(session, msg);
}

inline void msg::enter_world_msg::handle(msg_session_ref session, msg_reader reader)
{
    enter_world_msg msg;
    reader >> msg;
    handle(session, msg);
}

inline void msg::enter_world_msg::broadcast(const type& msg)
{
    for (listener_list::iterator iter = _listeners.begin(); iter != _listeners.end(); ++iter) {
        (*iter)(msg);
    }
}

inline void msg::move_msg::handle(msg_session_ref session, const type& msg)
{
    _ASSERT(_handler != NULL);
    _handler(session, msg);
}

inline void msg::move_msg::handle(msg_session_ref session, msg_reader reader)
{
    move_msg msg;
    reader >> msg;
    handle(session, msg);
}

inline void msg::move_msg::broadcast(const type& msg)
{
    for (listener_list::iterator iter = _listeners.begin(); iter != _listeners.end(); ++iter) {
        (*iter)(msg);
    }
}

inline void msg::update_position_msg::handle(msg_session_ref session, const type& msg)
{
    _ASSERT(_handler != NULL);
    _handler(session, msg);
}

inline void msg::update_position_msg::handle(msg_session_ref session, msg_reader reader)
{
    update_position_msg msg;
    reader >> msg;
    handle(session, msg);
}

inline void msg::update_position_msg::broadcast(const type& msg)
{
    for (listener_list::iterator iter = _listeners.begin(); iter != _listeners.end(); ++iter) {
        (*iter)(msg);
    }
}

inline void msg::despawn_msg::handle(msg_session_ref session, const type& msg)
{
    _ASSERT(_handler != NULL);
    _handler(session, msg);
}

inline void msg::despawn_msg::handle(msg_session_ref session, msg_reader reader)
{
    despawn_msg msg;
    reader >> msg;
    handle(session, msg);
}

inline void msg::despawn_msg::broadcast(const type& msg)
{
    for (listener_list::iterator iter = _listeners.begin(); iter != _listeners.end(); ++iter) {
        (*iter)(msg);
    }
}

inline void msg::chat_msg::handle(msg_session_ref session, const type& msg)
{
    _ASSERT(_handler != NULL);
    _handler(session, msg);
}

inline void msg::chat_msg::handle(msg_session_ref session, msg_reader reader)
{
    chat_msg msg;
    reader >> msg;
    handle(session, msg);
}

inline void msg::chat_msg::broadcast(const type& msg)
{
    for (listener_list::iterator iter = _listeners.begin(); iter != _listeners.end(); ++iter) {
        (*iter)(msg);
    }
}

inline void msg::character_resource_msg::handle(msg_session_ref session, const type& msg)
{
    _ASSERT(_handler != NULL);
    _handler(session, msg);
}

inline void msg::character_resource_msg::handle(msg_session_ref session, msg_reader reader)
{
    character_resource_msg msg;
    reader >> msg;
    handle(session, msg);
}

inline void msg::character_resource_msg::broadcast(const type& msg)
{
    for (listener_list::iterator iter = _listeners.begin(); iter != _listeners.end(); ++iter) {
        (*iter)(msg);
    }
}

inline void msg::interact_msg::handle(msg_session_ref session, const type& msg)
{
    _ASSERT(_handler != NULL);
    _handler(session, msg);
}

inline void msg::interact_msg::handle(msg_session_ref session, msg_reader reader)
{
    interact_msg msg;
    reader >> msg;
    handle(session, msg);
}

inline void msg::interact_msg::broadcast(const type& msg)
{
    for (listener_list::iterator iter = _listeners.begin(); iter != _listeners.end(); ++iter) {
        (*iter)(msg);
    }
}

inline void msg::update_hp_msg::handle(msg_session_ref session, const type& msg)
{
    _ASSERT(_handler != NULL);
    _handler(session, msg);
}

inline void msg::update_hp_msg::handle(msg_session_ref session, msg_reader reader)
{
    update_hp_msg msg;
    reader >> msg;
    handle(session, msg);
}

inline void msg::update_hp_msg::broadcast(const type& msg)
{
    for (listener_list::iterator iter = _listeners.begin(); iter != _listeners.end(); ++iter) {
        (*iter)(msg);
    }
}

inline void msg::alert_msg::handle(msg_session_ref session, const type& msg)
{
    _ASSERT(_handler != NULL);
    _handler(session, msg);
}

inline void msg::alert_msg::handle(msg_session_ref session, msg_reader reader)
{
    alert_msg msg;
    reader >> msg;
    handle(session, msg);
}

inline void msg::alert_msg::broadcast(const type& msg)
{
    for (listener_list::iterator iter = _listeners.begin(); iter != _listeners.end(); ++iter) {
        (*iter)(msg);
    }
}

inline void msg::spawn_msg::handle(msg_session_ref session, const type& msg)
{
    _ASSERT(_handler != NULL);
    _handler(session, msg);
    msg::character_resource_msg::handle(session, msg.character_resource);
    msg::update_position_msg::handle(session, msg.update_position);
    msg::update_hp_msg::handle(session, msg.update_hp);
}

inline void msg::spawn_msg::handle(msg_session_ref session, msg_reader reader)
{
    spawn_msg msg;
    reader >> msg;
    handle(session, msg);
}

inline void msg::spawn_msg::broadcast(const type& msg)
{
    for (listener_list::iterator iter = _listeners.begin(); iter != _listeners.end(); ++iter) {
        (*iter)(msg);
    }
}

inline void msg::world_info_msg::handle(msg_session_ref session, const type& msg)
{
    _ASSERT(_handler != NULL);
    _handler(session, msg);
    for (spawn_list::const_iterator iter = msg.spawns.begin(); iter != msg.spawns.end(); ++iter) {
        msg::spawn_msg::handle(session, *iter);
    }
}

inline void msg::world_info_msg::handle(msg_session_ref session, msg_reader reader)
{
    world_info_msg msg;
    reader >> msg;
    handle(session, msg);
}

inline void msg::world_info_msg::broadcast(const type& msg)
{
    for (listener_list::iterator iter = _listeners.begin(); iter != _listeners.end(); ++iter) {
        (*iter)(msg);
    }
}

}


#pragma warning (default: 4100)