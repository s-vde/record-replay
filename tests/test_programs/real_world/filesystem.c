
//--------------------------------------------------------------------------------------------------
/// @file filesystem.c
/// @brief Example program from @cite flanagan-popl-95 emulating a filesystem.
/// Demonstrates that static methods cannot always accurately predict dependencies.
/// @detail In the paper, NUM_BLOCKS = 26 and NUM_INODE = 32.
/// Races occur with NUM_THR > NUM_BLOCKS/2.
/// @author Susanne van den Elsen
/// @date 2015
//--------------------------------------------------------------------------------------------------

#include <pthread.h>

#define NUM_THR 14
#define NUM_BLOCKS 26
#define NUM_INODE 32

//--------------------------------------------------------------------------------------------------

pthread_mutex_t locki[NUM_INODE];
int inode[NUM_INODE];
pthread_mutex_t lockb[NUM_BLOCKS];
int busy[NUM_BLOCKS];

//--------------------------------------------------------------------------------------------------

void* thread(void* arg)
{
   int i = (*(int*)arg) % NUM_INODE; // pick an inode
   
   pthread_mutex_lock(locki + i);
   // If the inode does not already have an associated block, search for a free block to allocate to 
   // the inode.
   if (inode[i] == 0)
   {
      // Search starts at an arbitrary block index to avoid excessive lock contention.
      // b == n for all i in { (n + k(NUM_BLOCKS))/2 }
      int b = (i * 2) % NUM_BLOCKS;
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
         b = (b + 1) % NUM_BLOCKS;
      }
   }
   pthread_mutex_unlock(locki + i);
   
   pthread_exit(0);
}

//--------------------------------------------------------------------------------------------------

void initialize_inodes()
{
   for (int i = 0; i < NUM_INODE; ++i)
   {
      pthread_mutex_init(locki + i, NULL);
   }
}

//--------------------------------------------------------------------------------------------------

void initialize_blocks()
{
   for (int i = 0; i < NUM_BLOCKS; ++i)
   {
      pthread_mutex_init(lockb + i, NULL);
   }
}

//--------------------------------------------------------------------------------------------------

int main()
{
   initialize_inodes();
   initialize_blocks();

   pthread_t threads[NUM_THR];
   int tids[NUM_THR];
   for (int i = 0; i < NUM_THR; ++i)
   {
      tids[i] = i;
      pthread_create(threads + i, NULL, thread, tids + i);
   }
   
   for (int i = 0; i < NUM_THR; ++i)
   {
      pthread_join(threads[i], NULL);
   }

   return 0;
}

//--------------------------------------------------------------------------------------------------
