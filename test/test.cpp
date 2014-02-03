#include "gtest/gtest.h"

extern void ClientLogicTest();
extern void RenderingServiceTest();

int main(int argc, char **argv) {
    ClientLogicTest();
    RenderingServiceTest();

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}