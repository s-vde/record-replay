
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

/// @brief Array holding the status of the [NUM_THR] forks, where (1) means available and (0) means
/// taken
int fork[NUM_THR];

/// @brief Array of mutex locks for each fork
pthread_mutex_t lock[NUM_THR];

/// @brief Array holding number of meals consumed by each philosopher
int meals[NUM_THR];

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
   int right = (id + 1) % NUM_THR;

   int first = left;
   int second = right;

   // first fork is available
   if (take_up(id, first) == 1)
   {
      // second fork is also available
      if (take_up(id, second) == 1)
      {
         meals[id]++;
         put_down(id, first);
         put_down(id, second);
      }
      // second fork is not available
      else
      {
         put_down(id, first);
      }
   }
   
   pthread_exit(0);
}

//--------------------------------------------------------------------------------------------------

int main()
{
   pthread_t phils[NUM_THR];
   int pid[NUM_THR];
   void* status;

   for (int i = 0; i < NUM_THR; ++i)
   {
      pid[i] = i;
      fork[i] = 1;
      meals[i] = 0;
      pthread_mutex_init(lock + i, NULL);
   }
   for (int i = 0; i < NUM_THR; ++i)
   {
      pthread_create(phils + i, 0, philosopher, pid + i);
   }
   for (int i = 0; i < NUM_THR; ++i)
   {
      pthread_join(phils[i], &status);
   }
   return 0;
}

//--------------------------------------------------------------------------------------------------
