/* this code is auto-generated. */

#pragma once

#include <msg_def.h>
#include <msg_reader.h>
#include <msg_writer.h>
#include <msg_session_ref.h>

#pragma warning (disable: 4100)

namespace msg_type {
    const msg_type_t server = 4000;
    const msg_type_t request_server = 4001;
    const msg_type_t servers = 4002;
    const msg_type_t inter_chat_login = 4003;
    const msg_type_t inter_chat = 4004;
    const msg_type_t inter_chat_command = 4005;
    const msg_type_t alive_ping = 4006;
    const msg_type_t alive_pong = 4007;
}


namespace msg {

struct server_msg {
    static const msg_type_t __type = msg_type::server;
    
    std::string host;
    int port;
    std::string name;
    
    server_msg(std::string _host, int _port, std::string _name)
        : host(_host), port(_port), name(_name) {}
    
    #ifdef _DEBUG
    server_msg()
        : host(), port(0), name() {}
    #else
    server_msg() {}
    #endif
    
    typedef server_msg type;
    typedef std::function<void (msg_session_ref, const type&)> handler_t;
    typedef void (*listener_t)(const type&);
    typedef std::vector<listener_t> listener_list;
    
    static handler_t _handler;
    static listener_list _listeners;
    static void handle(msg_session_ref session, const type& msg);
    static void handle(msg_session_ref session, msg_reader reader);
    static void broadcast(const type& msg);
};

struct request_server_msg {
    static const msg_type_t __type = msg_type::request_server;
    
    
    request_server_msg() {}
    
    typedef request_server_msg type;
    typedef std::function<void (msg_session_ref, const type&)> handler_t;
    typedef void (*listener_t)(const type&);
    typedef std::vector<listener_t> listener_list;
    
    static handler_t _handler;
    static listener_list _listeners;
    static void handle(msg_session_ref session, const type& msg);
    static void handle(msg_session_ref session, msg_reader reader);
    static void broadcast(const type& msg);
};

struct inter_chat_login_msg {
    static const msg_type_t __type = msg_type::inter_chat_login;
    
    std::string name;
    
    inter_chat_login_msg(std::string _name)
        : name(_name) {}
    
    #ifdef _DEBUG
    inter_chat_login_msg()
        : name() {}
    #else
    inter_chat_login_msg() {}
    #endif
    
    typedef inter_chat_login_msg type;
    typedef std::function<void (msg_session_ref, const type&)> handler_t;
    typedef void (*listener_t)(const type&);
    typedef std::vector<listener_t> listener_list;
    
    static handler_t _handler;
    static listener_list _listeners;
    static void handle(msg_session_ref session, const type& msg);
    static void handle(msg_session_ref session, msg_reader reader);
    static void broadcast(const type& msg);
};

struct inter_chat_msg {
    static const msg_type_t __type = msg_type::inter_chat;
    
    std::string name;
    std::string message;
    long ticks;
    
    inter_chat_msg(std::string _name, std::string _message, long _ticks)
        : name(_name), message(_message), ticks(_ticks) {}
    
    #ifdef _DEBUG
    inter_chat_msg()
        : name(), message(), ticks(0) {}
    #else
    inter_chat_msg() {}
    #endif
    
    typedef inter_chat_msg type;
    typedef std::function<void (msg_session_ref, const type&)> handler_t;
    typedef void (*listener_t)(const type&);
    typedef std::vector<listener_t> listener_list;
    
    static handler_t _handler;
    static listener_list _listeners;
    static void handle(msg_session_ref session, const type& msg);
    static void handle(msg_session_ref session, msg_reader reader);
    static void broadcast(const type& msg);
};

struct inter_chat_command_msg {
    static const msg_type_t __type = msg_type::inter_chat_command;
    
    int type_code;
    std::string content;
    
    inter_chat_command_msg(int _type_code, std::string _content)
        : type_code(_type_code), content(_content) {}
    
    #ifdef _DEBUG
    inter_chat_command_msg()
        : type_code(0), content() {}
    #else
    inter_chat_command_msg() {}
    #endif
    
    typedef inter_chat_command_msg type;
    typedef std::function<void (msg_session_ref, const type&)> handler_t;
    typedef void (*listener_t)(const type&);
    typedef std::vector<listener_t> listener_list;
    
    static handler_t _handler;
    static listener_list _listeners;
    static void handle(msg_session_ref session, const type& msg);
    static void handle(msg_session_ref session, msg_reader reader);
    static void broadcast(const type& msg);
};

struct alive_ping_msg {
    static const msg_type_t __type = msg_type::alive_ping;
    
    
    alive_ping_msg() {}
    
    typedef alive_ping_msg type;
    typedef std::function<void (msg_session_ref, const type&)> handler_t;
    typedef void (*listener_t)(const type&);
    typedef std::vector<listener_t> listener_list;
    
    static handler_t _handler;
    static listener_list _listeners;
    static void handle(msg_session_ref session, const type& msg);
    static void handle(msg_session_ref session, msg_reader reader);
    static void broadcast(const type& msg);
};

struct alive_pong_msg {
    static const msg_type_t __type = msg_type::alive_pong;
    
    
    alive_pong_msg() {}
    
    typedef alive_pong_msg type;
    typedef std::function<void (msg_session_ref, const type&)> handler_t;
    typedef void (*listener_t)(const type&);
    typedef std::vector<listener_t> listener_list;
    
    static handler_t _handler;
    static listener_list _listeners;
    static void handle(msg_session_ref session, const type& msg);
    static void handle(msg_session_ref session, msg_reader reader);
    static void broadcast(const type& msg);
};

struct servers_msg {
    static const msg_type_t __type = msg_type::servers;
    
    typedef std::vector<msg::server_msg> server_list;
    server_list servers;
    
    servers_msg() {}
    
    typedef servers_msg type;
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
inline msg_writer& msg_writer::operator << (const msg::server_msg& msg)
{
    (*this) << msg_type::server;
    (*this) << msg.host;
    (*this) << msg.port;
    (*this) << msg.name;
    return (*this);
}

template <>
inline msg_reader& msg_reader::operator >> (msg::server_msg& msg)
{
    (*this) >> msg.host;
    (*this) >> msg.port;
    (*this) >> msg.name;
    return (*this);
}

template <>
inline msg_writer& msg_writer::operator << (const msg::request_server_msg& msg)
{
    (*this) << msg_type::request_server;
    return (*this);
}

template <>
inline msg_reader& msg_reader::operator >> (msg::request_server_msg& msg)
{
    return (*this);
}

template <>
inline msg_writer& msg_writer::operator << (const msg::inter_chat_login_msg& msg)
{
    (*this) << msg_type::inter_chat_login;
    (*this) << msg.name;
    return (*this);
}

template <>
inline msg_reader& msg_reader::operator >> (msg::inter_chat_login_msg& msg)
{
    (*this) >> msg.name;
    return (*this);
}

template <>
inline msg_writer& msg_writer::operator << (const msg::inter_chat_msg& msg)
{
    (*this) << msg_type::inter_chat;
    (*this) << msg.name;
    (*this) << msg.message;
    (*this) << msg.ticks;
    return (*this);
}

template <>
inline msg_reader& msg_reader::operator >> (msg::inter_chat_msg& msg)
{
    (*this) >> msg.name;
    (*this) >> msg.message;
    (*this) >> msg.ticks;
    return (*this);
}

template <>
inline msg_writer& msg_writer::operator << (const msg::inter_chat_command_msg& msg)
{
    (*this) << msg_type::inter_chat_command;
    (*this) << msg.type_code;
    (*this) << msg.content;
    return (*this);
}

template <>
inline msg_reader& msg_reader::operator >> (msg::inter_chat_command_msg& msg)
{
    (*this) >> msg.type_code;
    (*this) >> msg.content;
    return (*this);
}

template <>
inline msg_writer& msg_writer::operator << (const msg::alive_ping_msg& msg)
{
    (*this) << msg_type::alive_ping;
    return (*this);
}

template <>
inline msg_reader& msg_reader::operator >> (msg::alive_ping_msg& msg)
{
    return (*this);
}

template <>
inline msg_writer& msg_writer::operator << (const msg::alive_pong_msg& msg)
{
    (*this) << msg_type::alive_pong;
    return (*this);
}

template <>
inline msg_reader& msg_reader::operator >> (msg::alive_pong_msg& msg)
{
    return (*this);
}

template <>
inline msg_writer& msg_writer::operator << (const msg::servers_msg& msg)
{
    (*this) << msg_type::servers;
    (*this) << static_cast<uint32_t>(msg.servers.size());
    for (msg::servers_msg::server_list::const_iterator iter = msg.servers.begin(); iter != msg.servers.end(); ++iter) {
        (*this) << (*iter);
    }
    return (*this);
}

template <>
inline msg_reader& msg_reader::operator >> (msg::servers_msg& msg)
{
    uint32_t server_count;
    (*this) >> server_count;
    for (uint32_t index = 0; index < server_count; ++index) {
        msg::server_msg each;
        this->skip(static_cast<msg_size_t>(sizeof(msg_type_t)));
        (*this) >> each;
        msg.servers.push_back(each);
    }
    return (*this);
}

#pragma endregion


namespace msg {
inline void msg::server_msg::handle(msg_session_ref session, const type& msg)
{
    _ASSERT(_handler != NULL);
    _handler(session, msg);
}

inline void msg::server_msg::handle(msg_session_ref session, msg_reader reader)
{
    server_msg msg;
    reader >> msg;
    handle(session, msg);
}

inline void msg::server_msg::broadcast(const type& msg)
{
    for (listener_list::iterator iter = _listeners.begin(); iter != _listeners.end(); ++iter) {
        (*iter)(msg);
    }
}

inline void msg::request_server_msg::handle(msg_session_ref session, const type& msg)
{
    _ASSERT(_handler != NULL);
    _handler(session, msg);
}

inline void msg::request_server_msg::handle(msg_session_ref session, msg_reader reader)
{
    request_server_msg msg;
    reader >> msg;
    handle(session, msg);
}

inline void msg::request_server_msg::broadcast(const type& msg)
{
    for (listener_list::iterator iter = _listeners.begin(); iter != _listeners.end(); ++iter) {
        (*iter)(msg);
    }
}

inline void msg::inter_chat_login_msg::handle(msg_session_ref session, const type& msg)
{
    _ASSERT(_handler != NULL);
    _handler(session, msg);
}

inline void msg::inter_chat_login_msg::handle(msg_session_ref session, msg_reader reader)
{
    inter_chat_login_msg msg;
    reader >> msg;
    handle(session, msg);
}

inline void msg::inter_chat_login_msg::broadcast(const type& msg)
{
    for (listener_list::iterator iter = _listeners.begin(); iter != _listeners.end(); ++iter) {
        (*iter)(msg);
    }
}

inline void msg::inter_chat_msg::handle(msg_session_ref session, const type& msg)
{
    _ASSERT(_handler != NULL);
    _handler(session, msg);
}

inline void msg::inter_chat_msg::handle(msg_session_ref session, msg_reader reader)
{
    inter_chat_msg msg;
    reader >> msg;
    handle(session, msg);
}

inline void msg::inter_chat_msg::broadcast(const type& msg)
{
    for (listener_list::iterator iter = _listeners.begin(); iter != _listeners.end(); ++iter) {
        (*iter)(msg);
    }
}

inline void msg::inter_chat_command_msg::handle(msg_session_ref session, const type& msg)
{
    _ASSERT(_handler != NULL);
    _handler(session, msg);
}

inline void msg::inter_chat_command_msg::handle(msg_session_ref session, msg_reader reader)
{
    inter_chat_command_msg msg;
    reader >> msg;
    handle(session, msg);
}

inline void msg::inter_chat_command_msg::broadcast(const type& msg)
{
    for (listener_list::iterator iter = _listeners.begin(); iter != _listeners.end(); ++iter) {
        (*iter)(msg);
    }
}

inline void msg::alive_ping_msg::handle(msg_session_ref session, const type& msg)
{
    _ASSERT(_handler != NULL);
    _handler(session, msg);
}

inline void msg::alive_ping_msg::handle(msg_session_ref session, msg_reader reader)
{
    alive_ping_msg msg;
    reader >> msg;
    handle(session, msg);
}

inline void msg::alive_ping_msg::broadcast(const type& msg)
{
    for (listener_list::iterator iter = _listeners.begin(); iter != _listeners.end(); ++iter) {
        (*iter)(msg);
    }
}

inline void msg::alive_pong_msg::handle(msg_session_ref session, const type& msg)
{
    _ASSERT(_handler != NULL);
    _handler(session, msg);
}

inline void msg::alive_pong_msg::handle(msg_session_ref session, msg_reader reader)
{
    alive_pong_msg msg;
    reader >> msg;
    handle(session, msg);
}

inline void msg::alive_pong_msg::broadcast(const type& msg)
{
    for (listener_list::iterator iter = _listeners.begin(); iter != _listeners.end(); ++iter) {
        (*iter)(msg);
    }
}

inline void msg::servers_msg::handle(msg_session_ref session, const type& msg)
{
    _ASSERT(_handler != NULL);
    _handler(session, msg);
    for (server_list::const_iterator iter = msg.servers.begin(); iter != msg.servers.end(); ++iter) {
        msg::server_msg::handle(session, *iter);
    }
}

inline void msg::servers_msg::handle(msg_session_ref session, msg_reader reader)
{
    servers_msg msg;
    reader >> msg;
    handle(session, msg);
}

inline void msg::servers_msg::broadcast(const type& msg)
{
    for (listener_list::iterator iter = _listeners.begin(); iter != _listeners.end(); ++iter) {
        (*iter)(msg);
    }
}

}


#pragma warning (default: 4100)