
#include "replay.hpp"

#include "scheduler_settings.hpp"

#include <container_output.hpp>

#include <fstream>
#include <sys/stat.h>


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
