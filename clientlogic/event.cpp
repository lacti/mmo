#include "stdafx.h"
#include "clientlogic.h"
#include "event.h"
#include "ClContext.h"

typedef tbb::concurrent_queue<mmocl_event> event_queue;

inline ClContext* ClCtx();

namespace client
{
    class event_manager
    {
        static event_queue queue;

    public:
        static void push(const mmocl_event& e)
        {
            queue.push(e);
        }

        static bool try_pop(mmocl_event& e)
        {
            return queue.try_pop(e);
        }
    };
}

event_queue client::event_manager::queue;

void ClPushEvent(const mmocl_event& e)
{
    client::event_manager::push(e);

    if (ClCtx() && ClCtx()->event_pushed_event_handler)
    {
        ClCtx()->event_pushed_event_handler();
    }
}

CLIENTLOGIC_API int cl_poll_event(mmocl_event* event)
{
    if (event)
    {
        bool e = client::event_manager::try_pop(*event);

        return e ? CEC_OKAY : CEC_EVENT_EMPTY;
    }

    return CEC_NULL_PARAM;
}
