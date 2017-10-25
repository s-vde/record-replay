
#include "replay.hpp"

#include "scheduler_settings.hpp"

#include <container_output.hpp>
#include <fork.hpp>

#include <boost/filesystem.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <fstream>


namespace scheduler {

//--------------------------------------------------------------------------------------------------

void run_under_schedule(const program_t& program, const schedule_t& schedule,
                        const boost::optional<timeout_t>& timeout,
                        const boost::filesystem::path& output_dir)
{
   write_schedules(schedule);
   utils::sys::fork_process(program.string(), timeout);

   if (!boost::filesystem::exists(output_dir))
      boost::filesystem::create_directories(output_dir);

   boost::filesystem::rename("./record.txt", output_dir / "record.txt");
   boost::filesystem::rename("./record_short.txt", output_dir / "record_short.txt");
}

//--------------------------------------------------------------------------------------------------

void run_under_schedule(const program_t& program, const schedule_t& schedule,
                        const SchedulerSettings& settings,
                        const boost::optional<timeout_t>& timeout,
                        const boost::filesystem::path& output_dir)
{
   write_settings(settings);
   run_under_schedule(program, schedule, timeout, output_dir);
}

//--------------------------------------------------------------------------------------------------

namespace detail {

const static boost::filesystem::path llvm_bin = BOOST_PP_STRINGIZE(LLVM_BIN);
const static boost::filesystem::path record_replay_build_dir =
   BOOST_PP_STRINGIZE(RECORD_REPLAY_BUILD_DIR);

//--------------------------------------------------------------------------------------------------

std::string get_compiler(const program_t& program)
{
   std::string compiler;
   if (program.extension() == ".c")
      return "clang";
   else if (program.extension() == ".cpp")
      return "clang++";
   throw std::invalid_argument("Input program must be a .c or a .cpp program");
}

//--------------------------------------------------------------------------------------------------

boost::filesystem::path compile_to_llvm_ir(const program_t& program,
                                           const boost::filesystem::path& output_dir,
                                           const std::string& compiler,
                                           const std::string& optimization_level,
                                           const std::string& compiler_options)
{
   auto ir_program = output_dir / program.filename();
   ir_program += ".bc";

   const std::string command = (llvm_bin / compiler).string() + " -g -pthread -emit-llvm -O" +
                               optimization_level + " " + compiler_options + " -c " +
                               program.string() + " -o " + ir_program.string();
   system(command.c_str());

   return ir_program;
}

//--------------------------------------------------------------------------------------------------

boost::filesystem::path run_instrumentation_pass(const boost::filesystem::path& ir_program)
{
   auto instrumented_program = ir_program;
   instrumented_program.replace_extension(".instrumented.bc");

   const std::string command =
      (llvm_bin / "opt").string() + " -load " +
      (record_replay_build_dir / "src/llvm-pass/LLVMRecordReplayPass.dylib").string() +
      " -instrument-record-replay-lw < " + ir_program.string() + " > " +
      instrumented_program.string();
   system(command.c_str());

   return instrumented_program;
}

//--------------------------------------------------------------------------------------------------

void dump_human_readable_ir(const boost::filesystem::path& instrumented_ir)
{
   auto dump = instrumented_ir;
   dump.replace_extension(".txt");

   const std::string command =
      (llvm_bin / "llvm-dis").string() + " " + instrumented_ir.string() + " -o " + dump.string();
   system(command.c_str());
}

//--------------------------------------------------------------------------------------------------

boost::filesystem::path link_with_scheduler_library(const boost::filesystem::path& instrumented_ir,
                                                    const std::string& compiler)
{
   const auto scheduler_build_dir = record_replay_build_dir / "src/scheduler";
   auto instrumented_executable = instrumented_ir;
   instrumented_executable.replace_extension("");

   const std::string command =
      (llvm_bin / compiler).string() + " " + instrumented_ir.string() + " " +
      (scheduler_build_dir / "libRecordReplayScheduler.dylib").string() + " -rpath " +
      scheduler_build_dir.string() + " -o " + instrumented_executable.string();
   system(command.c_str());

   return instrumented_executable;
}

} // end namespace detail

//--------------------------------------------------------------------------------------------------

#if defined(LLVM_BIN) && defined(RECORD_REPLAY_BUILD_DIR)
boost::filesystem::path instrument(const program_t& program_source,
                                   const boost::filesystem::path& output_dir,
                                   const std::string& optimization_level,
                                   const std::string& compiler_options)
{
   if (boost::filesystem::exists(output_dir))
      boost::filesystem::remove_all(output_dir);
   boost::filesystem::create_directories(output_dir);

   const auto ir_program =
      detail::compile_to_llvm_ir(program_source, output_dir, detail::get_compiler(program_source),
                                 optimization_level, compiler_options);

   const auto instrumented_ir = detail::run_instrumentation_pass(ir_program);

   detail::dump_human_readable_ir(instrumented_ir);

   return detail::link_with_scheduler_library(instrumented_ir,
                                              detail::get_compiler(program_source));
}
#endif

//--------------------------------------------------------------------------------------------------

void write_settings(const SchedulerSettings& settings)
{
   system("test -d schedules || mkdir schedules");
   std::ofstream ofs("schedules/settings.txt");
   ofs << settings;
   ofs.close();
}

//--------------------------------------------------------------------------------------------------

void write_schedules(const schedule_t& schedule)
{
   system("test -d schedules || mkdir schedules");
   std::ofstream ofs("schedules/schedule.txt");
   ofs << schedule;
   ofs.close();
}

//--------------------------------------------------------------------------------------------------

} // end namespace scheduler
