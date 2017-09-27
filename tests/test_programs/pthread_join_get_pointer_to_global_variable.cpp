
#include <pthread.h>

#include <chrono>
#include <thread>


int global_variable = 0;

void* return_pointer_to_global_variable(void*)
{
   pthread_exit((void*)&global_variable);
}

void* modify_global_variable(void*)
{
   std::this_thread::sleep_for(std::chrono::milliseconds(2));
   global_variable = 1;
   pthread_exit(nullptr);
}

int main()
{
   pthread_t thread1;
   pthread_t thread2;
   int* pointer;

   pthread_create(&thread1, nullptr, &return_pointer_to_global_variable, nullptr);
   pthread_create(&thread2, nullptr, &modify_global_variable, nullptr);

   pthread_join(thread1, (void**)&pointer);
   *pointer = 2;
   pthread_join(thread2, nullptr);

   return 0;
}
