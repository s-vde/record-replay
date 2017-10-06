
#include <thread>

//--------------------------------------------------------------------------------------------------
/// @detail
/// Lifetime: A function static variable's lifetime begins when the program flow first encounters
/// its declaration and ends at the program's termination.
/// Thread-safety: The *initialization* of the static variable is thread-safe: i.e. accesses to
/// the variable happening concurrently with the initialization are waiting for the initialization
/// to complete. Subsequent accesses are *not* thread-safe.
//--------------------------------------------------------------------------------------------------


void initialize_static_variable(int value)
{
   static int static_variable = value;
   pthread_exit(0);
}

int main()
{
   std::thread thread1(initialize_static_variable, 1);
   std::thread thread2(initialize_static_variable, 2);

   thread1.join();
   thread2.join();

   return 0;
}
