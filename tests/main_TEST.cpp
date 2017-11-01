
#include "instrumentation_TEST.cpp"
#include "scheduler_TEST.cpp"
#include <execution_io_TEST.cpp>

#include <gtest/gtest.h>


int main(int argc, char** argv)
{
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
