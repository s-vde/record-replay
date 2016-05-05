
#include <fstream>
#include "replay.hpp"
#include "container_output.hpp"
#include "program.hpp"
#include "scheduler_settings.hpp"

namespace scheduler
{
    void run_under_schedule(const Program& B, const schedule_t& S)
    {
        write_schedules(B, S);
        system(B.to_string().c_str());
    }
    
    void run_under_schedule(
        const Program& B,
        const schedule_t& S,
        const SchedulerSettings& settings)
    {
        write_settings(settings);
        run_under_schedule(B, S);
    }
    
    void write_settings(const SchedulerSettings& settings)
    {
        std::ofstream ofs("schedules/settings.txt");
        ofs << settings;
        ofs.close();
    }
    
    void write_schedules(const Program& B, const schedule_t& S)
    {
        std::ofstream ofs("schedules/threads.txt");
        ofs << B.nr_threads();
        ofs.close();
        ofs.open("schedules/schedule.txt");
        ofs << S;
        ofs.close();
    }
} // end namespace scheduler
