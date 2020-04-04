/* leins, 18.11.2017 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "memallocator.h"

/* global pointers */
void *g_first;
void *g_begin, *g_end;
int g_maxSize;

/****************************
**| source functions |*******
****************************/
static int* GetSize(void *p)
{
  return (int*)p;
}/* end of function */
static void* GetNext(void *p)
{
  return (void*)((char*)p + sizeof(int));
}/* end of function */
static void* GetBlock(void *p)
{
  return (void*)((char*)p + memgetblocksize());
} /* end of function */

/*****************************
**| testing functions |*******
*****************************/
void PrintFree(void *p)
{
  int cnt = 0;

  printf("free blocks:\n");
  while (p != NULL)
  {
    int pSize = *GetSize(p);

    printf("\tsize %i - %i\n", cnt,  pSize);
    p = *(void**)GetNext(p);
    cnt++;
  }
  printf("\tNULL\n\n");
} /* end of function */

void PrintMemory(void *p, int maxSize)
{
  int size = maxSize;
  int cnt = 0;

  printf("all blocks:\n");
  while (size > 0)
  {
    int pSize = *GetSize(p);

    printf("\tsize %i - %i\n", cnt,  pSize);
    p = (char*)p + abs(pSize);
    size -= abs(pSize);
    cnt++;
  }
  printf("\tNULL\n\n");
} /* end of function */

/*************************************
**| functions from memalloc.h |*******
*************************************/
int memgetblocksize()
{
  return sizeof(int) + sizeof(void*);
} /* end of function */
int memgetminimumsize()
{
  return sizeof(int) + sizeof(void*);
} /* end of function */
int meminit(void *pMemory, int size)
{
  int blockSize = memgetblocksize();
  int returnSize = size - blockSize;

  if (size <  blockSize || pMemory == NULL)
    return returnSize;
  g_first = pMemory;
  *(int*)g_first = size;
  *(void**)GetNext(g_first) = NULL;
  g_begin = g_first;
  g_end = (void*)((char*)g_first + size);
  g_maxSize = size;

  return returnSize;
} /* end of function */

/**********************
**|  memalloc  |*******
**********************/

void *memalloc(int size)
{
  int totalSize = size + memgetblocksize();
  void *tmpList = g_first;
  void *theBest = tmpList;
  void *prev = NULL;
  void *blockToReturn = NULL;

  if (tmpList == NULL || size == 0)
    return NULL;
  // finding the best elem
  while (*GetSize(tmpList) != totalSize &&
         *(void**)GetNext(tmpList) != NULL)
  {
    if (*GetSize(*(void**)GetNext(tmpList)) == totalSize)
    {
      prev = tmpList;
      theBest = *(void**)GetNext(tmpList);
      break;
    }
    if (*GetSize(*(void**)GetNext(tmpList)) > totalSize)
      if (*GetSize(*(void**)GetNext(tmpList)) < *GetSize(theBest)
          || *GetSize(theBest) < totalSize)
      {
        prev = tmpList;
        theBest = *(void**)GetNext(tmpList);
      }
    tmpList = *(void**)GetNext(tmpList);
  }
  // break (if need) and return
  if (theBest != NULL)
  {
    void *tmp;

    if (*GetSize(theBest) < totalSize)
      return NULL;
    if (totalSize + memgetminimumsize() <= *GetSize(theBest))
    {
      tmp = (char*)theBest + totalSize;
      *GetSize(tmp) = *GetSize(theBest) - totalSize;
      *GetSize(theBest) = totalSize;
      *(void**)GetNext(tmp) = *(void**)GetNext(theBest);
    }
    else
      tmp = *(int**)GetNext(theBest);

    if (prev != NULL)
      *(void**)GetNext(prev) = tmp;
    else
      g_first = tmp;
    *GetSize(theBest) *= -1;
    blockToReturn = GetBlock(theBest);
  }
//  PrintMemory(g_begin, g_maxSize); //debug option
//  PrintFree(g_first); //debug option
  return blockToReturn;
} /* end of function */

/***************************
**| used in memfree |*******
***************************/
static void Connect(void *first, void *second)
{
  *GetSize(first) += *GetSize(second);
} /* end of function */
static void Add(void *first, void *second)
{
  *(void**)GetNext(second) = *(void**)GetNext(first);
  *(void**)GetNext(first) = second;
} /* end of function */
static int AddToEnd(void *left, void *p)
{
  int ret;
  if ((char*)left + *GetSize(left) == p)
  {
    Connect(left, p);
    ret = 1;
  }
  else
  {
    Add(left, p);
    ret = 0;
  }
  return ret;
} /* end of function */

static void ConnectList(void *left, void *right)
{
  *GetSize(left) += *GetSize(right);
  *(void**)GetNext(left) = *(void**)GetNext(right);
}
static void* AddToBegin(void *p, void *right)
{
  if ((char*)p + *GetSize(p) == right)
  {
    *GetSize(p) += *GetSize(right);
    *(int**)GetNext(p) = *(int**)GetNext(right);
  }
  else
    *(void**)GetNext(p) = right;
  return p;
} /* end of function */

/*
static void Defrag(void *p)
{
  //printf("%i\n", *GetSize(p));
  while (p != NULL && *(void**)GetNext(p) != NULL)
  {
    if((char*)p + *GetSize(p) == *(void**)GetNext(p))
      ConnectList(p, *(int**)GetNext(p));
    else
      p = *(int**)GetNext(p);
  }
} *//* end of function */

void memfree(void *p)
{
  void *tmpList = g_first;
  void *left = NULL, *right = NULL;
  void *prev = NULL, *rightPrev = NULL;

  if (p == NULL)
    return;
  p = (char*)p - memgetminimumsize();
  if (p < g_begin || p > g_end || *GetSize(p) >= 0)
    return;
  *GetSize(p) *= -1;
  while (tmpList != NULL)
  {
    if (tmpList < p)
      if (left == NULL || tmpList > left)
        left = tmpList;
    if (tmpList > p)
      if (right == NULL || tmpList < right)
      {
        rightPrev = prev;
        right = tmpList;
      }
    prev = tmpList;
    tmpList = *(void**)GetNext(tmpList);
  }

  if (left == NULL && right == NULL)
  {
    *(int**)GetNext(p) = NULL;
    g_first = p;
  }
  else if (right == NULL && left != NULL)
    AddToEnd(left, p);
  else if (right != NULL && left == NULL)
  {
    if (rightPrev != NULL)
      *(int**)GetNext(rightPrev) = AddToBegin(p, right);
    else
      g_first = AddToBegin(p, right);
  }
  else
  {
    if (AddToEnd(left, p))
    {
      if ((char*)left + *GetSize(left) == right)
        ConnectList(left, right);
    }
    else
    {
      if ((char*)p + *GetSize(p) == right)
        ConnectList(p, right);
    }
  }
//  Defrag(g_first);
//  PrintMemory(g_begin, g_maxSize); //debug option
//  PrintFree(g_first); //debug option
} /* end of function */

/* don't needed */
void memdone()
{
} /* end of function */
