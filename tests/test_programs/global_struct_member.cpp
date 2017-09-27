
#include <thread>


struct a_struct
{
   int member = 0;
} global_struct;

void function_modifying_global_struct(const int value)
{
   global_struct.member = value;
}

int main()
{
   std::thread thread1(function_modifying_global_struct, 1);
   std::thread thread2(function_modifying_global_struct, 2);

   thread1.join();
   thread2.join();

   return 0;
}
