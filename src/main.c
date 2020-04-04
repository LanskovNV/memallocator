
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "memallocator.h"

#define NUM_OF_BLOCKS 10
#define NUM_OF_ITERS 10000

int main(void)
{
  char *a;
  unsigned int i, k = 0, cnt = 0;
  unsigned int timer = (unsigned int)time(0);

  void *p[NUM_OF_BLOCKS];

  a = malloc((memgetminimumsize() + 2) * NUM_OF_BLOCKS);
  meminit(a, (memgetminimumsize() + 2) * NUM_OF_BLOCKS);

  for (i = 0; i < NUM_OF_ITERS; i++)
  {
    printf("%d %d ", i, k);
    srand(timer++);
    if ((rand() % 2 && k < NUM_OF_BLOCKS) || k == 0)
    {
      int rsize;

      printf("add\n");
      srand(timer++);
      rsize = rand() % 3;
      if (rsize == 0)
        rsize = 1;
      if ((p[k] = memalloc(rsize)) == NULL)
      {
       // printf("\n");
        cnt++;
      }
      k++;
    }
    else
    {
      unsigned int r, j;

      printf("free\n");
      srand(timer++);
      r = (int)(rand() / (float)RAND_MAX * (k - 1));

      memfree(p[r]);
      for (j = r; j < k; j++)
        p[j] = p[j + 1];
      k--;
    }
  }
  printf("%i \n", cnt);
  memdone();
  free(a);
  return 0;
}

/* End of 'main' function */

/* leins */
/*
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "memallocator.h"

#define SIZE 39
int main(void)
{
  void *p1, *p2, *p3;
  void *ptr;
  ptr = malloc(SIZE);
  meminit(ptr, SIZE);

  p1 = memalloc(1);
  p2 = memalloc(1);
  p3 = memalloc(1);
  memfree(p2);
  memfree(p3);
  p2 = memalloc(2);
  memfree(p2);
  memfree(p1);

  free(ptr);
  return 0;
}

*/
