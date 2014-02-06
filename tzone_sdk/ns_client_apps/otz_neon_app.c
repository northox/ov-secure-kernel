#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include "arm_neon.h"

/* fill array with increasing integers beginning with 0 */
void fill_array(int16_t *array, int size)
{    int i;
    for (i = 0; i < size; i++)
    {
         array[i] = i;
    }
}
/* return the sum of all elements in an array. This works by calculating 4 totals (one for each lane) and adding those at the end to get the final total */
int sum_array(int16_t *array, int size)
{
     /* initialize the accumulator vector to zero */
     int16x4_t acc = vdup_n_s16(0);
     int32x2_t acc1;
     int64x1_t acc2;
     /* this implementation assumes the size of the array is a multiple of 4 */
     assert((size % 4) == 0);
     /* counting backwards gives better code */
     for (; size != 0; size -= 4)
     {
          int16x4_t vec;
          /* load 4 values in parallel from the array */
          vec = vld1_s16(array);
          /* increment the array pointer to the next element */
          array += 4;
          /* add the vector to the accumulator vector */
          acc = vadd_s16(acc, vec);
      }
      /* calculate the total */
      acc1 = vpaddl_s16(acc);
     
      acc2 = vpaddl_s32(acc1);
      /* return the total as an integer */
      return (int)vget_lane_s64(acc2, 0);
}
/* main function */
int main()
{
      int16_t my_array[100];
      fill_array(my_array, 100);
      printf("Sum was %d\n", sum_array(my_array, 100));
      return 0;
}

