#include <stdio.h>
#include <stdlib.h>


typedef struct  {
    int max;
    int count;
    int rover;
    int *A2;
}Array_t;

// PROTOTYPES FROM OTHER HW QUESTIONS
//float Power(float x, int n);
//int Mult(int m, int n);
//int Min2(int* A, int k, int j);
//int Min(int*A);

void print_stats(Array_t*);

#define SIZE 5

int main(void)
{
    int A[] = {3, -1, 3, 1, 6}; 
    Array_t* Array = (Array_t*)malloc(sizeof(Array_t));

    Array->A2 = (int*)malloc(sizeof(int)*SIZE);
    
    for(int i=0; i<SIZE; i++) Array->A2[i] = A[i];
    
    Array->rover = 1;
    Array->count = 1;
    Array->max = A[0];

    print_stats(Array);
}

/* Q9 */
void print_stats(Array_t* Array)
{
    if(SIZE == 0)
	printf("max: 0\ncount: 0\n\n");
    else if(Array->rover == SIZE-1)
	printf("max: %d\ncount: %d\n\n", Array->max, Array->count);
    else if (Array->A2[Array->rover] == Array->max){
	Array->count++;
	Array->rover++;
	print_stats(Array);
    } else if (Array->A2[Array->rover] > Array->max){
	Array->count = 1;
	Array->max = Array->A2[Array->rover];
	Array->rover++;
	print_stats(Array);
    } else {
	Array->rover++;
	print_stats(Array);
    }

}

/* Q10
int Min(int*A)
{
    if(SIZE == 0)
	return 0;
    else if (SIZE == 1)
	return A[0];
    else
	return Min2(A,0,SIZE-1);
}

int Min2(int* A, int k, int j)
{
    if(j==k)
	return A[k];
    else if(A[k]<A[j])
	return Min2(A,k,j-1);
    else 
	return Min2(A,k+1,j-1);
}
*/

/*  Q1
float Power(float x, int n)
{
    if(n==0)
	return 1;
    else
        return (x*Power(x,n-1));
}*/

/* Q2
float Power(float x, int n)
{
    if(n==0)
	return 1;
    else if(n%2 == 0)
	return (Power(x,n/2) * Power(x,n/2));
    else {
        n = n-1;
        return (x*Power(x,n/2) * Power(x,n/2));
    }
}
*/

/* Q3 
int Mult(int m,int n)
{
    if(n!=0)
        return (m + Mult(m,n-1));
    else 
	return 0;
}*/
