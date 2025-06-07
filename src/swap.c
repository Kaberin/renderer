#include "swap.h"

void int_swap(int* x, int* y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}
void float_swap(float* x, float* y)
{
    float temp = *x;
    *x = *y;
    *y = temp;
}