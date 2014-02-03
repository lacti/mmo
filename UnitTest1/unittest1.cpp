#include "stdafx.h"
#include "CppUnitTest.h"
#include "../mmobase/mmobase.h"
#include "../clientlogic/clientlogic.h"
#include "game_msg.h"
#include "unittesthelper.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest1
{
    TEST_CLASS(UnitTest1)
    {
    public:
        TEST_METHOD(ContextInit)
        {
            mmocl_context ctx = -1;
            cl_initialize_context(&ctx, nullptr);
            Assert::AreNotEqual(mmocl_context(-1), ctx);
        }

        TEST_METHOD(ConnectToLocalhost)
        {
            mmocl_context ctx = -1;
            cl_initialize_context(&ctx, nullptr);
            Assert::AreNotEqual(mmocl_context(-1), ctx);
            AssertOkay(cl_connect_to_server("localhost", true));
            AssertOkay(cl_destroy_context());
        }

        TEST_METHOD(EnterWorld)
        {
            mmocl_context ctx = -1;
            cl_initialize_context(&ctx, nullptr);
            Assert::AreNotEqual(mmocl_context(-1), ctx);
            AssertOkay(cl_connect_to_server("localhost", true));
            AssertOkay(cl_enter_world("tester1"));
            AssertOkay(cl_destroy_context());
        }

        TEST_METHOD(StartNetworkThread)
        {
            mmocl_context ctx = -1;
            cl_initialize_context(&ctx, nullptr);
            Assert::AreNotEqual(mmocl_context(-1), ctx);
            AssertOkay(cl_connect_to_server("localhost", true));
            AssertOkay(cl_enter_world("tester2"));
            AssertOkay(cl_start_network_thread());
            msg::world_info_msg msg;
            Assert::IsTrue(wait_msg(msg, 5000), L"Make sure that the server is running");
            Assert::AreEqual(4, msg.id, L"Is the database reset?");
            AssertOkay(cl_destroy_context());
        }
    };
}