
#include "instrumentation_TEST.cpp"
#include <execution_io_TEST.cpp>


int main()
{
   program_model::test::execution_roundtrip_test();

   record_replay::test::instrumentation_test(
      "global_variable.cpp", "-std=c++14",
      {record_replay::test::wrapped_instruction_t{
         "call void @wrapper_post_memory_instruction(i32 1, i8* bitcast (i32* @global_variable to "
         "i8*), i8 0, i8* getelementptr inbounds ([96 x i8], [96 x i8]* "
         "@\"_recrep_file_name_[test_programs_dir]/global_variable.cpp\", i32 0, i32 0), i32 9)",
         "store i32 %2, i32* @global_variable, align 4, !dbg !1205"}});

   record_replay::test::instrumented_program_runs_through("global_variable.cpp", "-std=c++14");

   record_replay::test::instrumentation_test(
      "global_struct_member.cpp", "-std=c++14",
      {record_replay::test::wrapped_instruction_t{
         "call void @wrapper_post_memory_instruction(i32 1, i8* bitcast (%struct.a_struct* "
         "@global_struct to i8*), i8 0, i8* getelementptr inbounds ([101 x i8], [101 x i8]* "
         "@\"_recrep_file_name_[test_programs_dir]/global_struct_member.cpp\", i32 0, i32 0), i32 "
         "12)",
         "store i32 %2, i32* getelementptr inbounds (%struct.a_struct, %struct.a_struct* "
         "@global_struct, i32 0, i32 0), align 4, !dbg !1208"}});

   record_replay::test::instrumented_program_runs_through("global_struct_member.cpp", "-std=c++14");

   record_replay::test::instrumentation_test(
      "shared_variable_reference.cpp", "-std=c++14",
      {record_replay::test::wrapped_instruction_t{
         "call void @wrapper_post_memory_instruction(i32 1, i8* %4, i8 0, i8* "
         "getelementptr inbounds ([106 x i8], [106 x i8]* "
         "@\"_recrep_file_name_[test_programs_dir]/shared_variable_reference.cpp\", "
         "i32 0, i32 0), i32 9)",
         "store i32 1, i32* %ref, align 4, !dbg !2513"}});

   record_replay::test::instrumented_program_runs_through("shared_variable_reference.cpp",
                                                          "-std=c++14");

   record_replay::test::instrumentation_test(
      "function_static_variable_initialization.cpp", "-std=c++14",
      {
         // the initialization's store
         record_replay::test::wrapped_instruction_t{
            "call void @wrapper_post_memory_instruction(i32 1, i8* bitcast (i32* "
            "@_ZZ26initialize_static_variableiE15static_variable to i8*), i8 0, i8* getelementptr "
            "inbounds ([120 x i8], [120 x i8]* "
            "@\"_recrep_file_name_[test_programs_dir]/"
            "function_static_variable_initialization.cpp\", "
            "i32 0, i32 0), i32 16)",
            "store i32 %4, i32* @_ZZ26initialize_static_variableiE15static_variable, align 4, !dbg "
            "!1207"}
         // TODO: what about the __cxa_guard_acquire and __cxa_guard_release calls?
      });

   record_replay::test::instrumented_program_runs_through(
      "function_static_variable_initialization.cpp", "-std=c++14");

   record_replay::test::instrumentation_test(
      "function_static_variable.cpp", "-std=c++14",
      {record_replay::test::wrapped_instruction_t{
         "call void @wrapper_post_memory_instruction(i32 1, i8* bitcast (i32* "
         "@_ZZ22modify_static_variableiE15static_variable to i8*), i8 0, i8* getelementptr "
         "inbounds ([105 x i8], [105 x i8]* "
         "@\"_recrep_file_name_[test_programs_dir]/function_static_variable.cpp\", i32 0, i32 0), "
         "i32 17)",
         "store i32 %2, i32* @_ZZ22modify_static_variableiE15static_variable, align 4, !dbg "
         "!1203"}});

   record_replay::test::instrumented_program_runs_through("function_static_variable.cpp",
                                                          "-std=c++14");

   record_replay::test::instrumentation_test(
      "pthread_join_get_pointer_to_global_variable.cpp", "-std=c++14",
      {record_replay::test::wrapped_instruction_t{
         "call void @wrapper_post_memory_instruction(i32 1, i8* %8, i8 0, i8* getelementptr "
         "inbounds ([124 x i8], [124 x i8]* "
         "@\"_recrep_file_name_[test_programs_dir]/"
         "pthread_join_get_pointer_to_global_variable.cpp\", i32 0, i32 0), i32 32)",
         "store i32 2, i32* %7, align 4, !dbg !1379"}});

   record_replay::test::instrumented_program_runs_through(
      "pthread_join_get_pointer_to_global_variable.cpp", "-std=c++14");

   record_replay::test::instrumented_program_runs_through("real_world/filesystem.c", "");

   record_replay::test::instrumented_program_runs_through("real_world/dining_philosophers.c", "");

   record_replay::test::instrumented_program_runs_through("real_world/dining_philosophers.cpp",
                                                          "-std=c++14");

   record_replay::test::instrumented_program_runs_through("real_world/work_stealing_queue.cpp",
                                                          "-std=c++14");

   return 0;
}
