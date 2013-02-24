#ifndef _KISS_H_
#define _KISS_H_

#include <limits.h>

typedef unsigned long ulong;

void  kiss_seed(ulong seed);
ulong kiss_rand();
int   kiss_dX(int X);

#endif
