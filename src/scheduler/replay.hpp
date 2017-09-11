#pragma once

#include "schedule.hpp"

#include <string>

//--------------------------------------------------------------------------------------------------
/// @file replay.hpp
/// @brief Free functions providing an interface to the replay functionality of the Scheduler.
/// @author Susanne van den Elsen
/// @date 2015
//--------------------------------------------------------------------------------------------------


namespace scheduler {

// forward declarations
class SchedulerSettings;

//--------------------------------------------------------------------------------------------------


using program_t = std::string;

/// @brief Run the program under the settings currently in schedules/settings.txt.
/// @note If schedules/settings.txt does not exist, the Scheduler may not behave as
/// expected.

void run_under_schedule(const program_t&, const schedule_t&);

void run_under_schedule(const program_t&, const schedule_t&, const SchedulerSettings&);

//--------------------------------------------------------------------------------------------------


void write_settings(const SchedulerSettings&);

void write_schedules(const schedule_t&);

//--------------------------------------------------------------------------------------------------

} // end namespace scheduler
