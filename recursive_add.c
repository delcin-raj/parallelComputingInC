#include <stdio.h>
#include <mpi.h>

void main () {
    MPI_Init(NULL,NULL);
    int p, rank;
    MPI_Comm_size(MPI_COMM_WORLD,&p);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    int n = 16;
    int a[n];
    for (int i=0;i<n;i++){
        a[i] = i+1;
    }
    int sum = 0;

    // Important thing to note is that the processes with even rank should send
    // messages to the next process
    // The odd processes only receives the data first and send the partial sum back

    if (rank % 2 == 0) {
        if (rank == 0) {
            int k = n/2;
            for (int i = p/2 ; i > 0; i = i/2, k = k/2) {
                MPI_Send(a+i,k,MPI_INT,i,i+(i/2),MPI_COMM_WORLD);
            }
            // Doing its job
            for (int i = 0;i < n/p;i++) {
                sum += a[i];
            }

            // Receiving partial sum from other processes
            int sum_i;
            for (int i = p/2 ; i > 0; i = i/2, k = k/2) {
                MPI_Recv(&sum_i,1,MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            }
        } else {
            int limit;
            MPI_Recv(a,&limit,MPI_INT,)
        }

    }

    MPI_Finalize();
}
