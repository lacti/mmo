/* this code is auto-generated. */

#pragma once

#include <msg_def.h>
#include <msg_reader.h>
#include <msg_writer.h>
#include <msg_session_ref.h>

#pragma warning (disable: 4100)

namespace msg_type {
    const msg_type_t session_error = 2000;
}


namespace msg {

struct session_error_msg {
    static const msg_type_t __type = msg_type::session_error;
    
    int error_code;
    
    session_error_msg(int _error_code)
        : error_code(_error_code) {}
    
    #ifdef _DEBUG
    session_error_msg()
        : error_code(0) {}
    #else
    session_error_msg() {}
    #endif
    
    typedef session_error_msg type;
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
inline msg_writer& msg_writer::operator << (const msg::session_error_msg& msg)
{
    (*this) << msg_type::session_error;
    (*this) << msg.error_code;
    return (*this);
}

template <>
inline msg_reader& msg_reader::operator >> (msg::session_error_msg& msg)
{
    (*this) >> msg.error_code;
    return (*this);
}

#pragma endregion


namespace msg {
inline void msg::session_error_msg::handle(msg_session_ref session, const type& msg)
{
    _ASSERT(_handler != NULL);
    _handler(session, msg);
}

inline void msg::session_error_msg::handle(msg_session_ref session, msg_reader reader)
{
    session_error_msg msg;
    reader >> msg;
    handle(session, msg);
}

inline void msg::session_error_msg::broadcast(const type& msg)
{
    for (listener_list::iterator iter = _listeners.begin(); iter != _listeners.end(); ++iter) {
        (*iter)(msg);
    }
}

}


#pragma warning (default: 4100)