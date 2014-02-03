/* this code is auto-generated. */

#pragma once

#include <msg_def.h>
#include <msg_reader.h>
#include <msg_writer.h>
#include <msg_session_ref.h>

#pragma warning (disable: 4100)

namespace msg_type {
    const msg_type_t voice = 3000;
}


namespace msg {

struct voice_msg {
    static const msg_type_t __type = msg_type::voice;
    
    int id;
    msg::bin_t mp3;
    
    voice_msg(int _id, msg::bin_t _mp3)
        : id(_id), mp3(_mp3) {}
    
    #ifdef _DEBUG
    voice_msg()
        : id(0), mp3() {}
    #else
    voice_msg() {}
    #endif
    
    typedef voice_msg type;
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
inline msg_writer& msg_writer::operator << (const msg::voice_msg& msg)
{
    (*this) << msg_type::voice;
    (*this) << msg.id;
    (*this) << msg.mp3;
    return (*this);
}

template <>
inline msg_reader& msg_reader::operator >> (msg::voice_msg& msg)
{
    (*this) >> msg.id;
    (*this) >> msg.mp3;
    return (*this);
}

#pragma endregion


namespace msg {
inline void msg::voice_msg::handle(msg_session_ref session, const type& msg)
{
    _ASSERT(_handler != NULL);
    _handler(session, msg);
}

inline void msg::voice_msg::handle(msg_session_ref session, msg_reader reader)
{
    voice_msg msg;
    reader >> msg;
    handle(session, msg);
}

inline void msg::voice_msg::broadcast(const type& msg)
{
    for (listener_list::iterator iter = _listeners.begin(); iter != _listeners.end(); ++iter) {
        (*iter)(msg);
    }
}

}


#pragma warning (default: 4100)