#include "stdio.h"
#include "stdlib.h"
#include "mpi.h"

// For ascending order, to get descending order multiply by -1
int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

void merge (int *array_p,int *array_r,int k,int lower_half) {
    int temp[k];
    for (int i=0; i<k; i++) {
        temp[i] = array_p[i];
    }
    if (lower_half) {
        int i=0,j=0;
        for (int d=0; d<k; d++) {
            if ( j == k || temp[i] < array_r[j] ) {
                array_p[d] = temp[i++];
            } else {
                array_p[d] = array_r[j++];
            }
        }
    } else {
        // Upper half larger values
        int i = k-1,j = k-1;
        for (int d = k-1; d >= 0; d--) {
            if (j == 0 || temp[i] > array_r[j]) {
                array_p[d] = temp[i--];
            } else {
                array_p[d] = array_r[j--];
            }
        }
    }
}

int main () {
   int n = 64;
   int array[n];
   MPI_Init(NULL,NULL);
   int p,rank;
   // p stores the value of number of processes working
   // rank stores the value of the identity of each process
   MPI_Comm_size(MPI_COMM_WORLD,&p);
   MPI_Comm_rank(MPI_COMM_WORLD,&rank);
   MPI_Status status;
   int k = n/p;

   // partial array assigned for each processor
   int array_p[k];
   // to be received from neighbor
   int array_r[k];
   srand(rank);
   for(int i = 0 ; i < k; i++ ) {
       array_p[i] = rand()%100;
   }

   // printf("The original array is\n" );
   // for (int i = 0; i<k; i++) {
   //     printf("%d->",array_p[i]);
   // }
   qsort(array_p, k, sizeof(int), cmpfunc);
   int odd_neighbor,even_neighbor;

   if (rank % 2 == 0){
       odd_neighbor = rank - 1;
       even_neighbor = rank + 1;
   } else {
       odd_neighbor = rank + 1;
       even_neighbor = rank - 1;
   }

   if (rank == 0 || rank == p-1) {
       odd_neighbor = MPI_PROC_NULL;
   }

   for (int i = 0; i < p; i++) {
       // even phase
       if (i % 2 == 0) {
           MPI_Sendrecv(array_p,k,MPI_INT,even_neighbor,0,
               array_r,k,MPI_INT,even_neighbor,0,MPI_COMM_WORLD,&status);
       } else {
           MPI_Sendrecv(array_p,k,MPI_INT,odd_neighbor,0,
               array_r,k,MPI_INT,odd_neighbor,0,MPI_COMM_WORLD,&status);
       }
       // At each merge step the array_p is modified with respect to array_r
       // After merging we either need first half or the second half depending
       // on the rank of it's neighbor
       merge(array_p,array_r,k,rank < status.MPI_SOURCE);

   }

   if (rank != 0) {
       // Send the results to 0
       MPI_Send(array_p,k,MPI_INT,0,1,MPI_COMM_WORLD);
   } else if(rank == 0) {
       int array[n];
       int d = 0;
       int temp[k];
       printf("The sorted array is \n" );
       for (int j = 0;j < k;j++) {
           array[d++] = array_p[j];
       }
       for (int i = 1; i<p; i++) {
           MPI_Recv(temp,k,MPI_INT,i,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
           for (int j = 0;j < k;j++) {
               array[d++] = temp[j];
           }
       }
       for(d=0;d<n;d++) {
           printf("%d->",array[d] );
       }
   }

   MPI_Finalize();

   return(0);
}
