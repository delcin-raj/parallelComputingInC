#include "stdio.h"
#include "stdlib.h"
#include "mpi.h"

void main () {
    MPI_Init (NULL, NULL);
    int rank,p;
    MPI_Comm_size(MPI_COMM_WORLD,&p);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    int n = 32;
    int partial = n/p;

    /* Here we are doing matrix multiplication interms of column sum. Hence
       we don't have to gather the complete vector
    */
    int A_partial[n][partial];
    int b_partial[partial];
    // Each process computes partial solution. Since we are adding up the sums
    // The size of the vecor is n nonetheless
    int Ab_partial[n];
    int Ab[n]; // Solution vector
    srand(rank);

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < partial; j++) {
            A_partial [i][j] = rand() % 100;
            if (i == 0) {
                b_partial[j] = rand() % 99;
            }
        }
        Ab_partial[i] = 0; // Initialising the solution vecotr
    }

    // Ab has n rows
    for (int i=0; i < n; i++) {
        for (int j = 0; j < partial; j++) {
            Ab_partial[i] = Ab_partial[i] + b_partial[j] * A_partial[i][j];
        }
    }

    // Now the partial sums are to be added and stored in the solution buffer of process 0
    MPI_Reduce(Ab_partial,Ab,n,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);

    MPI_Finalize();
}
