#include "stdio.h"
#include "mpi.h"
#include "stdlib.h"

void main () {
    MPI_Init(NULL,NULL);
    int p,rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&p);
    int n = 32; // size of the array
    int partial = n/p;

    /* Each process has partial amount of rows in the augmented matrix [A b]
       partial rows by n colummns each.
    */

    int A_partial[partial][n];
    int b_partial[partial];
    int b[n]; //complete matrix
    int Ab[n]; //result matrix
    srand(rank);

    // Generation of partial data
    for (int i = 0; i < partial; i++) {
        for (int j = 0; j < n; j++) {
            A_partial[i][j] = rand() % 100;
        }
        b[i] = rand() % 1000;
    }

    /* Inorder to compute the matrix vector multiplication we need the entire
       vector b. We gather that data from the rest of the processes
     */

     MPI_Allgather(b_partial,partial,MPI_INT,b,partial,MPI_INT,MPI_COMM_WORLD);

     // Computing partial rows of Ab by each processor
     for (int i = rank * partial,k=0; i < (rank+1) * partial; i++,k++) {
         for (int j = 0; j < n; j++) {
             Ab[i] = Ab[i] + A_partial[k][j] * b[j];
         }
     }

     MPI_Finalize();

}
