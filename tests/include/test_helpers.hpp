#pragma once

#include <boost/filesystem/path.hpp>
#include <boost/preprocessor/stringize.hpp>


namespace record_replay {
namespace test {
namespace detail {

//--------------------------------------------------------------------------------------------------

static const auto test_programs_dir =
   boost::filesystem::path(BOOST_PP_STRINGIZE(TEST_PROGRAMS_DIR));
static const auto tests_build_dir = boost::filesystem::path{BOOST_PP_STRINGIZE(TESTS_BUILD_DIR)};
static const auto test_data_dir = tests_build_dir / "test_data";

} // end namespace detail

//--------------------------------------------------------------------------------------------------


struct InstrumentedProgramTestData
{
   boost::filesystem::path test_program;
   std::string optimization_level;
   std::string compiler_options;

}; // end struct InstrumentedProgramTestData

//--------------------------------------------------------------------------------------------------

} // end namespace test
} // end namespace record_replay
