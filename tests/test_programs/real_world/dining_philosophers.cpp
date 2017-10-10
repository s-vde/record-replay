
//--------------------------------------------------------------------------------------------------
/// @file dining-philosophers.cpp
/// @detail Classic scenario where a number of philosophers are at a dining table. After thinking
/// for a while each philosopher tries to eat their spaghetti. For that they need two forks: the
/// one to their left, and the one to their right. Each of the forks can be held by one philosopher
/// at a time.
/// @note This program has a potential deadlock.
/// @author Susanne van den Elsen
/// @date 2017
//--------------------------------------------------------------------------------------------------

#include <array>
#include <thread>

#ifndef NR_THREADS
   #define NR_THREADS 2
#endif

//--------------------------------------------------------------------------------------------------

struct fork
{
   fork() { pthread_mutex_init(&m_mutex, NULL); }

   void take_up() { pthread_mutex_lock(&m_mutex); }

   void put_down() { pthread_mutex_unlock(&m_mutex); }

private:
   pthread_mutex_t m_mutex;

}; // end struct fork

using forks_t = std::array<fork, NR_THREADS>;
using nr_meals_t = std::array<unsigned int, NR_THREADS>;
using philosophers_t = std::array<std::thread, NR_THREADS>;

//--------------------------------------------------------------------------------------------------

/// @brief Philosopher thread start routine

void philosopher(const unsigned int id, forks_t& forks, nr_meals_t& nr_meals)
{
   int left = id;
   int right = (id + 1) % NR_THREADS;

   forks[left].take_up();
   forks[right].take_up();
   ++nr_meals[id];
   forks[right].put_down();
   forks[left].put_down();
}

//--------------------------------------------------------------------------------------------------

int main()
{
   forks_t forks;
   nr_meals_t nr_meals;
   philosophers_t philosophers;
   
   for (unsigned int id = 0; id < NR_THREADS; ++id)
   {
      forks[id] = fork();
      nr_meals[id] = 0;
      philosophers[id] = std::thread(philosopher, id, std::ref(forks), std::ref(nr_meals));
   }

   for (auto& philosopher : philosophers)
   {
      philosopher.join();
   }

   return 0;
}

//--------------------------------------------------------------------------------------------------
