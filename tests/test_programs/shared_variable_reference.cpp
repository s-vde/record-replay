
#include <thread>


int main()
{
   int shared_variable = 0;
   std::thread spawn_thread([&shared_variable]() {
      shared_variable = 1;
      pthread_exit(0);
   });
   spawn_thread.join();
   return 0;
}
