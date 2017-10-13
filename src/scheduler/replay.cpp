
#include "replay.hpp"

#include "scheduler_settings.hpp"

#include <container_output.hpp>

#include <boost/preprocessor/stringize.hpp>

#include <sys/stat.h>

#include <fstream>


namespace scheduler {

//--------------------------------------------------------------------------------------------------

void run_under_schedule(const program_t& program, const schedule_t& schedule)
{
   write_schedules(schedule);
   system(program.c_str());
}

//--------------------------------------------------------------------------------------------------

void run_under_schedule(const program_t& program, const schedule_t& schedule,
                        const SchedulerSettings& settings)
{
   write_settings(settings);
   run_under_schedule(program, schedule);
}

//--------------------------------------------------------------------------------------------------

#if defined(LLVM_BIN) && defined(RECORD_REPLAY_BUILD_DIR)
void instrument(const program_t& program, const std::string& output_dir,
                const std::string& optimization_level, const std::string& compiler_options)
{
   const static std::string llvm_bin = BOOST_PP_STRINGIZE(LLVM_BIN);
   const static std::string record_replay_build_dir = BOOST_PP_STRINGIZE(RECORD_REPLAY_BUILD_DIR);
   const static std::string instrument_script = record_replay_build_dir + "/instrument.sh";
   const std::string command = instrument_script + " " + record_replay_build_dir + " " + llvm_bin +
                               " " + program + " " + output_dir + " " + optimization_level + " " +
                               compiler_options;
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
