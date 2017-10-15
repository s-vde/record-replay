
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

#if defined(LLVM_BIN) && defined(RECORD_REPLAY_BUILD_DIR)
void instrument(const program_t& program, const boost::filesystem::path& output_dir,
                const std::string& optimization_level, const std::string& compiler_options)
{
   const static std::string llvm_bin = BOOST_PP_STRINGIZE(LLVM_BIN);
   const static std::string record_replay_build_dir = BOOST_PP_STRINGIZE(RECORD_REPLAY_BUILD_DIR);
   const static std::string instrument_script = record_replay_build_dir + "/instrument.sh";
   const std::string command = instrument_script + " " + record_replay_build_dir + " " + llvm_bin +
                               " " + program.string() + " " + output_dir.string() + " " +
                               optimization_level + " " + compiler_options;
   system(command.c_str());
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
