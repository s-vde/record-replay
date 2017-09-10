
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
      {0, next_t{memory_instruction{0, memory_operation::Load, Object(&var_1), false, {"test_file", 1}}, true}},
      {1, next_t{memory_instruction{1, memory_operation::Store, Object(&var_2), true, {"test_file", 2}}, true}}
   };
   const auto state_0 = std::shared_ptr<State>(new State({0, 1}, next_0));
   
   // state_1
   NextSet next_1 = {
      {0, next_t{lock_instruction{0, lock_operation::Lock, Object(&mut_1), {"test_file", 3}}, true}},
      {1, next_t{memory_instruction{1, memory_operation::Store, Object(&var_2), true, {"test_file", 2}}, true}}
   };
   const auto state_1 = std::shared_ptr<State>(new State({0, 1}, next_1));
   
   // state_2
   NextSet next_2 = {
      {0, next_t{lock_instruction{0, lock_operation::Lock, Object(&mut_1), {"test_file", 3}}, true}},
      {1, next_t{lock_instruction{1, lock_operation::Lock, Object(&mut_1), {"test_file", 5}}, true}}
   };
   const auto state_2 = std::shared_ptr<State>(new State({0, 1}, next_2));
   
   // state_3
   NextSet next_3 = {
      {0, next_t{lock_instruction{0, lock_operation::Unlock, Object(&mut_1), {"test_file", 4}}, true}},
      {1, next_t{lock_instruction{1, lock_operation::Lock, Object(&mut_1), {"test_file", 5}}, false}}
   };
   const auto state_3 = std::shared_ptr<State>(new State({0}, next_3));
   
   // state_4
   NextSet next_4 = {
      {1, next_t{lock_instruction{1, lock_operation::Lock, Object(&mut_1), {"test_file", 5}}, true}}
   };
   const auto state_4 = std::shared_ptr<State>(new State({0}, next_4));
   
   // state_5
   NextSet next_5 = {
      {1, next_t{lock_instruction{1, lock_operation::Unlock, Object(&mut_1), {"test_file", 6}}, true}}
   };
   const auto state_5 = std::shared_ptr<State>(new State({0}, next_5));
   
   const auto state_6 = std::shared_ptr<State>(new State({}, {}));
   
   Execution execution_write{2, state_0};
   execution_write.push_back(next_0[0].instr, state_1);
   execution_write.push_back(next_1[1].instr, state_2);
   execution_write.push_back(next_2[0].instr, state_3);
   execution_write.push_back(next_3[0].instr, state_4);
   execution_write.push_back(next_4[1].instr, state_5);
   execution_write.push_back(next_5[1].instr, state_6);
   
   {
      std::ofstream output_file("execution_io_TEST.txt");
      output_file << execution_write;
   }
   
   Execution execution_read = Execution(2);
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