
//--------------------------------------------------------------------------------------------------
/// @file dining-philosophers.c
/// @detail Classic scenario where a number of philosophers are at a dining table. After thinking
/// for a while each philosopher tries to eat their spaghetti. For that they need two forks: the
/// one to their left, and the one to their right. Each of the forks can be held by one philosopher
/// at a time.
/// @note This program has a potential deadlock.
/// @author Susanne van den Elsen
/// @date 2017
//--------------------------------------------------------------------------------------------------

#include <pthread.h>

#ifndef NR_THREADS
   #define NR_THREADS 2
#endif

//--------------------------------------------------------------------------------------------------

/// @brief Array holding the status of the [NR_THREADS] forks, where (1) means available and (0) means
/// taken
int fork[NR_THREADS];

/// @brief Array of mutex locks for each fork
pthread_mutex_t lock[NR_THREADS];

/// @brief Array holding number of meals consumed by each philosopher
int meals[NR_THREADS];

//--------------------------------------------------------------------------------------------------

int take_up(int pid, int fid)
{
   pthread_mutex_lock(lock + fid);
   int success = 0;
   if (fork[fid] == 1)
   {
      fork[fid] = 0;
      success = 1;
   }
   pthread_mutex_unlock(lock + fid);
   return success;
}

//--------------------------------------------------------------------------------------------------

void put_down(int pid, int fid)
{
   pthread_mutex_lock(lock + fid);
   fork[fid] = 1;
   pthread_mutex_unlock(lock + fid);
}

//--------------------------------------------------------------------------------------------------

void* philosopher(void* arg)
{
   int id = *(int*)arg;
   int left = id;
   int right = (id + 1) % NR_THREADS;

   // left fork is available
   if (take_up(id, left) == 1)
   {
      // right fork is also available
      if (take_up(id, right) == 1)
      {
         meals[id]++;
         put_down(id, left);
         put_down(id, right);
      }
      // left fork is not available
      else
      {
         put_down(id, left);
      }
   }
   
   pthread_exit(0);
}

//--------------------------------------------------------------------------------------------------

int main()
{
   pthread_t phils[NR_THREADS];
   int pid[NR_THREADS];
   void* status;

   for (int i = 0; i < NR_THREADS; ++i)
   {
      pid[i] = i;
      fork[i] = 1;
      meals[i] = 0;
      pthread_mutex_init(lock + i, NULL);
   }
   for (int i = 0; i < NR_THREADS; ++i)
   {
      pthread_create(phils + i, 0, philosopher, pid + i);
   }
   for (int i = 0; i < NR_THREADS; ++i)
   {
      pthread_join(phils[i], &status);
   }
   return 0;
}

//--------------------------------------------------------------------------------------------------
