/*
  Example of multi-language programming: C function is called from M2
*/

#include <stdio.h>

int c_func(int a, int b)
{
        printf("\nThis is a C function called by M2 procedure\n");
        printf("c_func (%d, %d)\n", a, b);
        return a+b; 
}

