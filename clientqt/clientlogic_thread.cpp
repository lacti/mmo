#include "stdafx.h"
#include "clientlogic_thread.h"
#include "../clientlogic/clientlogic.h"

void debug_line_output_handler(const char* line);
void chat_output_handler(int id, const char* line);

clientlogic_thread::clientlogic_thread()
    : m_alive(true)
{
    m_command_queue.set_capacity(128);
}

void clientlogic_thread::run()
{
    cl_initialize_params params;
    params.chat_output_handler = chat_output_handler;
    params.debug_line_output_handler = debug_line_output_handler;
    
    mmocl_context ctx;
    cl_initialize_context(&ctx, &params);
    
    while (is_alive())
    {   
        QElapsedTimer et;
        double frame_time = 0;

        logic_update(frame_time);
        frame_time = static_cast<decltype(frame_time)>(et.nsecsElapsed()) / 1e9;
    }

    cl_destroy_context();
}

void clientlogic_thread::logic_update(const double delta)
{
    while (!m_command_queue.empty())
    {
        clt_async_command_t command;
        m_command_queue.pop(command);
        
        if (command)
        {
            command();
        }
    }
    
    cl_frame_move(delta);
}
