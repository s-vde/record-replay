
#include <replay.hpp>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <assert.h>
#include <sys/stat.h>

#include <chrono>
#include <exception>
#include <fstream>


namespace record_replay {
namespace test {

struct wrapped_instruction_t
{
   std::string wrapper_pre;
   std::string instruction;
}; // end struct wrapper_instruction_t


namespace detail {

//--------------------------------------------------------------------------------------------------

static const auto test_programs_dir =
   boost::filesystem::path(BOOST_PP_STRINGIZE(TEST_PROGRAMS_DIR));
static const auto tests_build_dir = boost::filesystem::path{BOOST_PP_STRINGIZE(TESTS_BUILD_DIR)};
static const auto output_dir = tests_build_dir / "test_data" / "test_programs_instrumented";

inline bool is_instrumented(const boost::filesystem::path& test_program,
                            const wrapped_instruction_t& wrapped_instruction)
{
   auto ir_dump = output_dir / test_program;
   ir_dump += ".instrumented.txt";

   std::ifstream ifs{ir_dump.string()};
   std::string previous, current;
   while (previous = current, std::getline(ifs, current))
   {
      if (current == "  " + wrapped_instruction.instruction)
      {
         return previous == "  " + wrapped_instruction.wrapper_pre;
      }
   }
   throw std::runtime_error("EXCEPTION: Instruction " + wrapped_instruction.instruction +
                            " not found");
}

//--------------------------------------------------------------------------------------------------

} // end namespace detail


inline void instrumentation_test(const boost::filesystem::path& test_program,
                                 const std::string& compiler_options,
                                 std::vector<wrapped_instruction_t> expected_wrapped_instructions)
{
   scheduler::instrument((detail::test_programs_dir / test_program).string(),
                         detail::output_dir.string(), compiler_options);

   std::for_each(expected_wrapped_instructions.begin(), expected_wrapped_instructions.end(),
                 [&test_program](auto& wrapped_instruction) {
                    boost::replace_first(wrapped_instruction.wrapper_pre, "[test_programs_dir]",
                                         detail::test_programs_dir.string());
                    assert(detail::is_instrumented(test_program, wrapped_instruction));
                 });
}

//--------------------------------------------------------------------------------------------------

inline void instrumented_program_runs_through(const boost::filesystem::path& test_program,
                                              const std::string& compiler_options)
{
   scheduler::instrument((detail::test_programs_dir / test_program).string(),
                         detail::output_dir.string(), compiler_options);

   scheduler::run_under_schedule((detail::output_dir / test_program.stem()).string(), {});
}

//--------------------------------------------------------------------------------------------------

} // end namespace test
} // end namespace record_replay
