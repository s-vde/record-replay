
#include <thread>


int global_variable = 0;

void modify_global_variable(const int value)
{
   global_variable = value;
   pthread_exit(0);
}

int main()
{
   std::thread thread1(modify_global_variable, 1);
   std::thread thread2(modify_global_variable, 2);

   thread1.join();
   thread2.join();

   return 0;
}
