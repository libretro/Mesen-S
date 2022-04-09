/* 7zAlloc.c -- Allocation functions
2015-11-09 : Igor Pavlov : Public domain */

#include "Precomp.h"

#include "7zAlloc.h"

void *SzAlloc(void *p, size_t size)
{
  UNUSED_VAR(p);
  if (size == 0)
    return 0;
  return malloc(size);
}

void SzFree(void *p, void *address)
{
  UNUSED_VAR(p);
  free(address);
}

void *SzAllocTemp(void *p, size_t size)
{
  UNUSED_VAR(p);
  if (size == 0)
    return 0;
  return malloc(size);
}

void SzFreeTemp(void *p, void *address)
{
  UNUSED_VAR(p);
  free(address);
}
