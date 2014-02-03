#include "gtest/gtest.h"
#include "../mmobase/mmobase.h"
#include "../clientlogic/clientlogic.h"
#include "../clientlogic/event.h"

void ClientLogicTest() {}

inline void AssertOkay(int result)
{
    ASSERT_EQ(int(CEC_OKAY), result);
}

TEST(ClientLogicTest, EnterWorld)
{
    mmocl_context ctx = -1;
    cl_initialize_context(&ctx, nullptr);
    ASSERT_NE(mmocl_context(-1), ctx);
    AssertOkay(cl_connect_to_server("localhost", true));
    AssertOkay(cl_enter_world("tester"));
    ASSERT_EQ(0, cl_player_id());
    AssertOkay(cl_start_network_thread());
    int TryCount = 10000000;
    while (TryCount-- >= 0)
    {
        cl_poll_network_service();

        mmocl_event cl_event;
        if (cl_poll_event(&cl_event) == CEC_OKAY)
        {
            ASSERT_EQ(MET_ENTER_WORLD, cl_event.type);
            ASSERT_EQ(1, cl_event.enter_world.world_id);
            break;
        }
    }
    ASSERT_LT(0, TryCount);
    AssertOkay(cl_destroy_context());
}
