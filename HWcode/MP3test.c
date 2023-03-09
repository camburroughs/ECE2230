#include <stdio.h>
#include <stdlib.h>
typedef    int    InputArray[6];
void IterativeSelectionSort(InputArray A, int m, int n);

int main(void)
{
    InputArray A = {1,7,8,3,12,10};
    IterativeSelectionSort(A,0,6);
    for(int i=0; i <6; i++)
        printf("%d,", A[i]);
    printf("\n");
}

void IterativeSelectionSort(InputArray A, int m, int n)
{
   int MaxPosition, temp, i;

   while (m < n) {
   
      i = m; 
      MaxPosition = m;

      do {
         i++;
         if ( A[i] > A[MaxPosition] ) MaxPosition = i;
      } while (i != n);
   
      temp = A[m]; A[m] = A[MaxPosition]; A[MaxPosition] = temp;
   
      m++;
   }
}