#pragma once

#include "schedule.hpp"

#include <string>

//--------------------------------------------------------------------------------------------------
/// @file replay.hpp
/// @brief Free functions providing an interface to the replay functionality of the Scheduler.
/// @author Susanne van den Elsen
/// @date 2015-2017
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

#if defined(LLVM_BIN) && defined(RECORD_REPLAY_BUILD_DIR)
void instrument(const program_t&, const std::string& output_dir,
                const std::string& optimization_level = "0",
                const std::string& compiler_options = "");
#endif

void write_settings(const SchedulerSettings&);

void write_schedules(const schedule_t&);

//--------------------------------------------------------------------------------------------------

} // end namespace scheduler
