
#include <replay.hpp>

#include <gtest/gtest.h>

#include <boost/filesystem.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <chrono>

//--------------------------------------------------------------------------------------------------

namespace record_replay {
namespace test {
namespace detail {

static const auto test_programs_dir =
   boost::filesystem::path(BOOST_PP_STRINGIZE(TEST_PROGRAMS_DIR));
static const auto tests_build_dir = boost::filesystem::path{BOOST_PP_STRINGIZE(TESTS_BUILD_DIR)};
static const auto test_data_dir = tests_build_dir / "test_data";

} // end namespace detail

//--------------------------------------------------------------------------------------------------

struct InstrumentedProgramRunTestData
{
   boost::filesystem::path test_program;
   std::string optimization_level;
   std::string compiler_options;

}; // end struct InstrumentedProgramRunTestData

struct InstrumentedProgramRunTest : public ::testing::TestWithParam<InstrumentedProgramRunTestData>
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
}; // end struct InstrumentedProgramRunTest

TEST_P(InstrumentedProgramRunTest, InstrumentedProgramRunsThrough)
{
   const auto instrumented_executable = scheduler::instrument(
      detail::test_programs_dir / GetParam().test_program, test_output_dir() / "instrumented",
      GetParam().optimization_level, GetParam().compiler_options);

   ASSERT_NO_THROW(scheduler::run_under_schedule(
      instrumented_executable, {}, std::chrono::milliseconds(3000), test_output_dir() / "records"));
}

INSTANTIATE_TEST_CASE_P(
   RealWorldPrograms, InstrumentedProgramRunTest,
   ::testing::Values(
      InstrumentedProgramRunTestData{"real_world/filesystem.c", "0", ""},
      InstrumentedProgramRunTestData{"real_world/dining_philosophers.c", "0", ""},
      InstrumentedProgramRunTestData{"real_world/dining_philosophers.cpp", "0", "-std=c++14"},
      InstrumentedProgramRunTestData{"real_world/work_stealing_queue.cpp", "0", "-std=c++14"}));

//--------------------------------------------------------------------------------------------------

} // end namespace test
} // end namespace record_replay
