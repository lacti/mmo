#pragma once

#include "clientlogic.h"
#include "entity_manager_ref.h"
#include "msg_session_ref.h"

static void null_debug_output_handler(const char* line) {}
static void null_chat_output_handler(int id, const char* line) {}
static void null_game_event_handler(int event_type) {}
static void null_event_pushed_event_handler() {}

struct ClContext
{
    ClContext()
        : io_svc_thread(nullptr)
        , debug_output_handler(null_debug_output_handler)
        , chat_output_handler(null_chat_output_handler)
        , game_event_handler(null_game_event_handler)
		, event_pushed_event_handler(null_event_pushed_event_handler)
        , left(false)
        , right(false)
        , up(false)
        , down(false)
        , jump(false)
    {
    }

    boost::asio::io_service     io_svc;
    std::thread*                io_svc_thread;
    msg_session_ref             session;
    cl_fn_debug_output_handler  debug_output_handler;
    cl_fn_chat_output_handler   chat_output_handler;
    cl_fn_game_event_handler    game_event_handler;
	cl_fn_event_pushed_event_handler    event_pushed_event_handler;
    client::entity_manager_ref  em;
    class LogicWorld*           lw;
    bool                        left, right, up, down, jump;
};
