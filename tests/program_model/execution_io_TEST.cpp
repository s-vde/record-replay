
#include <execution_io.hpp>

#include <fstream>
#include <thread>

#include <assert.h>


namespace program_model {
namespace test {

void execution_roundtrip_test()
{
   int var_1 = 0;
   std::atomic<int> var_2;
   var_2.store(0);
   std::mutex mut_1;
   
   // state_0
   NextSet next_0 = {
      {0, next_t{thread_management_instruction{0, thread_management_operation::Spawn, Thread(1), {"test_file", 1}}, true}},
   };
   const auto state_0 = std::shared_ptr<State>(new State({0}, next_0));
   
   // state_1
   NextSet next_1 = {
      {0, next_t{memory_instruction{0, memory_operation::Load, Object(&var_1), false, {"test_file", 2}}, true}},
      {1, next_t{memory_instruction{1, memory_operation::Store, Object(&var_2), true, {"test_file", 3}}, true}}
   };
   const auto state_1 = std::shared_ptr<State>(new State({0, 1}, next_1));
   
   // state_2
   NextSet next_2 = {
      {0, next_t{lock_instruction{0, lock_operation::Lock, Object(&mut_1), {"test_file", 4}}, true}},
      {1, next_t{memory_instruction{1, memory_operation::Store, Object(&var_2), true, {"test_file", 3}}, true}}
   };
   const auto state_2 = std::shared_ptr<State>(new State({0, 1}, next_2));
   
   // state_3
   NextSet next_3 = {
      {0, next_t{lock_instruction{0, lock_operation::Lock, Object(&mut_1), {"test_file", 4}}, true}},
      {1, next_t{lock_instruction{1, lock_operation::Lock, Object(&mut_1), {"test_file", 6}}, true}}
   };
   const auto state_3 = std::shared_ptr<State>(new State({0, 1}, next_3));
   
   // state_4
   NextSet next_4 = {
      {0, next_t{lock_instruction{0, lock_operation::Unlock, Object(&mut_1), {"test_file", 5}}, true}},
      {1, next_t{lock_instruction{1, lock_operation::Lock, Object(&mut_1), {"test_file", 6}}, false}}
   };
   const auto state_4 = std::shared_ptr<State>(new State({0}, next_4));
   
   // state_5
   NextSet next_5 = {
      {0, next_t{thread_management_instruction{0, thread_management_operation::Join, Thread(1), {"test_file", 8}}, false}},
      {1, next_t{lock_instruction{1, lock_operation::Lock, Object(&mut_1), {"test_file", 6}}, true}}
   };
   const auto state_5 = std::shared_ptr<State>(new State({0}, next_5));
   
   // state_6
   NextSet next_6 = {
      {0, next_t{thread_management_instruction{0, thread_management_operation::Join, Thread(1), {"test_file", 8}}, false}},
      {1, next_t{lock_instruction{1, lock_operation::Unlock, Object(&mut_1), {"test_file", 7}}, true}}
   };
   const auto state_6 = std::shared_ptr<State>(new State({1}, next_6));
   
   // state_7
   NextSet next_7 = {
      {0, next_t{thread_management_instruction{0, thread_management_operation::Join, Thread(1), {"test_file", 8}}, true}}
   };
   const auto state_7 = std::shared_ptr<State>(new State({0}, next_7));
   
   const auto state_8 = std::shared_ptr<State>(new State({}, {}));
   
   Execution execution_write{state_0};
   execution_write.push_back(next_0[0].instr, state_1); // spawn
   execution_write.push_back(next_1[0].instr, state_2); // load
   execution_write.push_back(next_2[1].instr, state_3); // store
   execution_write.push_back(next_3[0].instr, state_4); // lock
   execution_write.push_back(next_4[0].instr, state_5); // unlock
   execution_write.push_back(next_5[1].instr, state_6); // lock
   execution_write.push_back(next_6[1].instr, state_7); // unlock
   execution_write.push_back(next_7[0].instr, state_8); // join
   
   {
      std::ofstream output_file("execution_io_TEST.txt");
      output_file << execution_write;
   }
   
   Execution execution_read{};
   {
      std::ifstream input_file("execution_io_TEST.txt");
      input_file >> execution_read;
   }
   assert(execution_write == execution_read);
}

//--------------------------------------------------------------------------------------------------

} // end namespace test
} // end namespace program_model


int main()
{
   program_model::test::execution_roundtrip_test();
   return 0;
}
