
//--------------------------------------------------------------------------------------------------
/// @file filesystem.c
/// @brief Example program from @cite flanagan-popl-95 emulating a filesystem.
/// Demonstrates that static methods cannot always accurately predict dependencies.
/// @detail In the paper, NR_BLOCKS = 26 and NR_INODE = 32.
/// Races occur with NR_THREADS > NR_BLOCKS/2.
/// @author Susanne van den Elsen
/// @date 2015
//--------------------------------------------------------------------------------------------------

#include <pthread.h>

#ifndef NR_THREADS
   #define NR_THREADS 14
#endif
#define NR_BLOCKS 26
#define NR_INODE 32

//--------------------------------------------------------------------------------------------------

pthread_mutex_t locki[NR_INODE];
int inode[NR_INODE];
pthread_mutex_t lockb[NR_BLOCKS];
int busy[NR_BLOCKS];

//--------------------------------------------------------------------------------------------------

void* thread(void* arg)
{
   int i = (*(int*)arg) % NR_INODE; // pick an inode
   
   pthread_mutex_lock(locki + i);
   // If the inode does not already have an associated block, search for a free block to allocate to 
   // the inode.
   if (inode[i] == 0)
   {
      // Search starts at an arbitrary block index to avoid excessive lock contention.
      // b == n for all i in { (n + k(NR_BLOCKS))/2 }
      int b = (i * 2) % NR_BLOCKS;
      while (1)
      {
         pthread_mutex_lock(lockb + b);
         if (!busy[b])
         {
            busy[b] = 1;
            inode[i] = b + 1;
            pthread_mutex_unlock(lockb + b);
            break;
         }
         pthread_mutex_unlock(lockb + b);
         b = (b + 1) % NR_BLOCKS;
      }
   }
   pthread_mutex_unlock(locki + i);
   
   pthread_exit(0);
}

//--------------------------------------------------------------------------------------------------

void initialize_inodes()
{
   for (int i = 0; i < NR_INODE; ++i)
   {
      pthread_mutex_init(locki + i, NULL);
   }
}

//--------------------------------------------------------------------------------------------------

void initialize_blocks()
{
   for (int i = 0; i < NR_BLOCKS; ++i)
   {
      pthread_mutex_init(lockb + i, NULL);
   }
}

//--------------------------------------------------------------------------------------------------

int main()
{
   initialize_inodes();
   initialize_blocks();

   pthread_t threads[NR_THREADS];
   int tids[NR_THREADS];
   for (int i = 0; i < NR_THREADS; ++i)
   {
      tids[i] = i;
      pthread_create(threads + i, NULL, thread, tids + i);
   }
   
   for (int i = 0; i < NR_THREADS; ++i)
   {
      pthread_join(threads[i], NULL);
   }

   return 0;
}

//--------------------------------------------------------------------------------------------------
