#include "stdio.h"
#include "mpi.h"

void main() {
    MPI_Init(NULL,NULL);
    int p,rank;
    MPI_Comm_size(MPI_COMM_WORLD,&p);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int n = 16;
    int a[n];
    for (int i=0;i<n;i++){
        a[i] = i;
    }
    int sum = 0;

    if (n % p == 0) {
        int k = n/p;
        //printf("%d,%d\n",p ,k);
        if (rank == 0) {
            // Dividing up the work to p processes
            for (int i = 1;i <p;i++) {
                // initially I have missed i*k with only k which caused wrong output
                MPI_Send(a+i*k,k,MPI_INT,i,0,MPI_COMM_WORLD);
            }

            // Doing it's work
            for (int i = 0;i < k; i++) {
                sum += a[i];
            }

            // Receiving the partial sum from the rest of the processes
            for (int i = 1;i < p;i++) {
                int sum_i;
                MPI_Recv(&sum_i,1,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                // Accumulating the sum
                sum += sum_i;
            }
            printf("%d\n",sum );

        } else {
            int sum_i = 0;
            MPI_Recv (a,k,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            for (int i = 0;i < k;i++) {
                sum_i += a[i];
            }
            MPI_Send(&sum_i,1,MPI_INT,0,0,MPI_COMM_WORLD);
            printf("%d\n",sum_i );
        }


    }

    MPI_Finalize();

}
