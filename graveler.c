#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/time.h>
#include <pthread.h>

const int MAX_ITERATIONS = 1000 * 1000 * 1000;
const int THREAD_COUNT = 4;

void *helperThread(void *pGreatest)
{
  int nHighest;
  for (int i = 0; i < MAX_ITERATIONS / THREAD_COUNT; i++)
  {
    int nCounter = 0;
    // Computes 14 * 16 = 224 attempts.
    for (int j = 0; j < 14; j++)
    {
      // Assumes the random number generated is at least 32 bits.
      int nNumber = rand();
      for (int k = 0; k < 16; k++)
      {
        nCounter += (nNumber >> (2 * k)) & (nNumber >> (2 * k + 1)) & 1;
      }
    }
    // Needs 7 more attempts.
    int nNumber = rand();
    for (int k = 0; k < 7; k++)
    {
      nCounter += (nNumber >> (2 * k)) & (nNumber >> (2 * k + 1)) & 1;
    }
    // This relies on integer overflow to work properly.
    int nSelector = ((nCounter - nHighest) & INT_MIN) == 0;
    nHighest = (nHighest * (~nSelector + 2)) + (nCounter * nSelector);
  }
  *(int*)pGreatest = nHighest;
  pthread_exit(NULL);
}

int main() {
  struct timeval tStartTime;
  gettimeofday(&tStartTime, NULL);

  srand(time(0));

  pthread_t threads[THREAD_COUNT];
  int threadResults[THREAD_COUNT];
  for (int i = 0; i < THREAD_COUNT; i++)
  {
    pthread_create(threads + i, NULL, helperThread, (void*)(threadResults + i));
  }
  for (int i = 0; i < THREAD_COUNT; i++)
  {
    pthread_join(threads[i], NULL);
  }

  int nHighest = 0;
  for (int i = 0; i < THREAD_COUNT; i++)
  {
    if (threadResults[i] > nHighest)
    {
      nHighest = threadResults[i];
    }
  }

  struct timeval tEndTime;
  gettimeofday(&tEndTime, NULL);

  printf("Elapsed Time: %f\n", (double)(tEndTime.tv_sec - tStartTime.tv_sec) + (double)(tEndTime.tv_usec - tStartTime.tv_usec) / 1000 / 1000);
  printf("Highest Ones Roll: %d\n", nHighest);
  printf("Number of Roll Sessions: %d\n", MAX_ITERATIONS);
}
