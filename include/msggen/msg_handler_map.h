#pragma once

#include "msg_def.h"
#include "msg_reader.h"
#include "msg_session.h"

// msg handler function pointer
typedef void (*msg_handler)(msg_session_ref, msg_reader);

// msg handler map (in msg session) auto register
struct msg_handler_map_register {
    msg_handler_map_register(msg_type_t msg_type, msg_handler handler);
};

inline msg_handler_map_register::msg_handler_map_register(msg_type_t msg_type, msg_handler handler)
{
    // this array implemented in msg_session.ipp
    extern msg_handler msg_handler_map[65536];
    if (msg_handler_map[msg_type] != NULL)
        _ASSERT(FALSE);

    msg_handler_map[msg_type] = handler;
}

// msg handler (in msg class) auto register
template <typename _MsgTy>
struct msg_handler_register {
    msg_handler_register(void (*handler)(msg_session_ref, const _MsgTy&));
};

template <typename _MsgTy>
inline msg_handler_register<_MsgTy>::msg_handler_register(void (*handler)(msg_session_ref, const _MsgTy&))
{
    // set msg handler
    _MsgTy::_handler = handler;
}


// helper macro for msg handler stub-code generation
#define MSG_HANDLER_FUNCTION_NAME(msg_name) \
    msg_name##_handler

#define MSG_CLASS_NAME(msg_name)    \
    msg::msg_name##_msg

#define DECLARE_MSG_HANDLER_FUNCTION(msg_name) \
    static void MSG_HANDLER_FUNCTION_NAME(msg_name) (msg_session_ref session, const MSG_CLASS_NAME(msg_name)& msg)

#define DEFINE_MSG_MAP_REGISTER(msg_name) \
	static msg_handler_map_register __##msg_name##_map_register(msg_type::msg_name, MSG_CLASS_NAME(msg_name)::handle)

#define REGISTER_MSG_MAP_HANDLER(msg_name)    \
    DECLARE_MSG_HANDLER_FUNCTION(msg_name); \
    DEFINE_MSG_MAP_REGISTER(msg_name) \

#define DEFINE_MSG_HANDLER(msg_name) \
	MSG_CLASS_NAME(msg_name)::handler_t MSG_CLASS_NAME(msg_name)::_handler

#define REGISTER_MSG_HANDLER(msg_name)  \
    DEFINE_MSG_HANDLER(msg_name); \
    static msg_handler_register<MSG_CLASS_NAME(msg_name)> __##msg_name##_register(MSG_HANDLER_FUNCTION_NAME(msg_name))

#define DEFINE_TEST_MSG_HANDLER(msg_name) \
	DEFINE_MSG_HANDLER(msg_name); \
	DEFINE_MSG_MAP_REGISTER(msg_name);

// msg handler stub-code generation macro
#define MSG_HANDLER(msg_name)    \
    REGISTER_MSG_MAP_HANDLER(msg_name); \
    REGISTER_MSG_HANDLER(msg_name); \
    DECLARE_MSG_HANDLER_FUNCTION(msg_name)

/*
    // use msg handler. args(session, msg)
    MSG_HANDLER(user_login)
    {
        LAPP_ << "welcome " << msg.name;
    }
*/

#define UNUSE_MSG(msg_name) \
    MSG_HANDLER(msg_name) { session; msg; }
