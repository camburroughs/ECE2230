/* C Programs from Chapter 5 of
 * Data Structures, Algorithms and Software Principles in C
 * by Thomas A. Standish
 * Copyright (C) 1995, Addison-Wesley, Inc., all rights reserved
 */


typedef    int    InputArray[6];

int FindMax(InputArray A, int m, int n)   /* assume m<n */
{
   int i = m;      /* i is an index that visits all positions from m to n */
   int j = m;     /* j is an index that saves the position of the largest */
            
                     
   do {
      i++;                      /* advance i to point to next number A[i] */
      if (A[i] > A[j]) {          /* if A[i] > largest previous A[j] then */
         j = i;              /* save the position, i, of the largest in j */
      }
   } while (i != n);           /* stop when all i in m:n have been tested */

   return j;                       /* return j == position of the largest */
}                                                /* number A[j] in A[m:n] */


void SelectionSort(InputArray A, int m, int n)
{
   int   MaxPosition;    /* MaxPosition is the index of A's biggest item  */
   int   temp;                     /* temp is used to exchange items in A */

   if (m < n) {               /* if there is more than one number to sort */
   
      /* Let MaxPosition be the index of the largest number in A[m:n] */
         MaxPosition = FindMax(A,m,n);
      
      /* Exchange A[m] <--> A[MaxPosition] */
         temp = A[m];
         A[m] = A[MaxPosition];
         A[MaxPosition] = temp;
      
      /* SelectionSort the subarray A[m+1:n] */
         SelectionSort(A, m+1, n);
   }
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

/*
void MergeSort(ListType List)
{
   if (the List has more than one item in it) {
      (break the List into two half-lists, L = LeftList and R = RightList)
      (sort the LeftList using MergeSort(L))
      (sort the RightList using MergeSort(R))
      (merge L and R into a single sorted List)
   } else {
      (do nothing, since the list is already sorted)
   }
}
*/
