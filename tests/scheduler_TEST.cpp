
#include "include/test_helpers.hpp"

#include <replay.hpp>

#include <gtest/gtest.h>

#include <boost/filesystem.hpp>

#include <chrono>

//--------------------------------------------------------------------------------------------------

namespace record_replay {
namespace test {

struct SchedulerDeadlockSanitityCheck : public ::testing::TestWithParam<InstrumentedProgramTestData>
{
   void SetUp() override
   {
      boost::filesystem::create_directories(
         detail::test_data_dir / GetParam().test_program.filename() /
         boost::filesystem::path("0" + GetParam().optimization_level) / "records");
   }

   boost::filesystem::path test_output_dir() const
   {
      return detail::test_data_dir / GetParam().test_program.filename() /
             boost::filesystem::path("0" + GetParam().optimization_level);
   }
}; // end struct SchedulerDeadlockSanitityCheck

TEST_P(SchedulerDeadlockSanitityCheck, SchedulerDoesNotEndInDeadlockOnMultipleRuns)
{
   const auto instrumented_executable = scheduler::instrument(
      detail::test_programs_dir / GetParam().test_program, test_output_dir() / "instrumented",
      GetParam().optimization_level, GetParam().compiler_options);

   for (int i = 0; i < 500; ++i)
      ASSERT_NO_THROW(scheduler::run_under_schedule(instrumented_executable, {},
                                                    std::chrono::milliseconds(3000),
                                                    test_output_dir() / "records"));
}

INSTANTIATE_TEST_CASE_P(
   RealWorldPrograms, SchedulerDeadlockSanitityCheck,
   ::testing::Values(                                                                       //
      InstrumentedProgramTestData{"real_world/dining_philosophers.cpp", "3", "-std=c++14"}, //
      InstrumentedProgramTestData{"real_world/work_stealing_queue.cpp", "3", "-std=c++14"}  //
      ));

//--------------------------------------------------------------------------------------------------

} // end namespace test
} // end namespace record_replay
