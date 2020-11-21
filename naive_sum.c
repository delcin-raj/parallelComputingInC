#include "stdio.h"
#include "mpi.h"
#include "stdlib.h"

/* Memory efficient version
   Data is generated only by process 0 of array of size n
   The rest of the processes are only gnerate an array of required size (n/p)
 */

void main() {
    MPI_Init(NULL,NULL);
    int p,rank;
    // p stores the value of number of processes working
    // rank stores the value of the identity of each process
    MPI_Comm_size(MPI_COMM_WORLD,&p);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    int n;
    n = 5;

    int sum = 0;
    int k = n/p;



    if (n % p == 0) {
        // Evenly dividing the jobs

        //printf("%d,%d\n",p ,k);
        if (rank == 0) {
            printf("This program prints the sum of first n natural numbers\n");
            // Dividing up the work to p processes
            int array[n];
            for (int i=0;i<n;i++){
                array[i] = i;
            }
            for (int i = 1;i <p;i++) {
                // initially I have missed i*k with only k which caused wrong output
                MPI_Send(array+i*k,k,MPI_INT,i,0,MPI_COMM_WORLD);
            }

            // Doing it's work
            for (int i = 0;i < k; i++) {
                sum += array[i];
            }

            // Receiving the partial sum from the rest of the processes
            for (int i = 1;i < p;i++) {
                int sum_i;
                // The constant MPI_ANY_SOURCE allows the process 0 to receive
                // messages from the remaining processes in any order
                MPI_Recv(&sum_i,1,MPI_INT,MPI_ANY_SOURCE,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                // Accumulating the sum
                sum += sum_i;
            }
            printf("Sum through MPI %d\n",sum );
            printf("Actual sum is %d", (n*(n-1))/2);

        } else {
            int sum_i = 0;
            int *a = malloc(k*sizeof(int));
            MPI_Recv (a,k,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            for (int i = 0;i < k;i++) {
                sum_i += a[i];
            }
            MPI_Send(&sum_i,1,MPI_INT,0,0,MPI_COMM_WORLD);

        }


    } else {
        // Evenly dividing the jobs for the first p-1 processses
        // Then giving the remaining task to the last process
        int k = n /(p-1);
        if (rank == 0) {
            int array[n];
            for (int i=0;i<n;i++){
                array[i] = i;
            }
            // Dividing up the work to p-1 processes from 1 to p-2
            for (int i = 1;i <p-1;i++) {
                MPI_Send(array+i*k,k,MPI_INT,i,0,MPI_COMM_WORLD);
            }

            // for the pth process (rank is p-1)

            MPI_Send(array+(p-1)*k, n - (p-1) * k,MPI_INT,p-1,0,MPI_COMM_WORLD);

            // Doing it's work
            for (int i = 0;i < k; i++) {
                sum += array[i];
            }

            // Receiving the partial sum from the rest of the processes
            for (int i = 1;i < p;i++) {
                int sum_i;
                MPI_Recv(&sum_i,1,MPI_INT,i,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
                // Accumulating the sum
                sum += sum_i;
            }
            printf("Sum through MPI %d\n",sum );
            printf("Actual sum is %d", (n*(n-1))/2);

        }
        // Accessible code block for processes 1 through p-2
        else if (rank < p-1) {
            int sum_i = 0;
            int *a = malloc(k*sizeof(int));
            MPI_Recv (a,k,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            for (int i = 0;i < k;i++) {
                sum_i += a[i];
            }
            MPI_Send(&sum_i,1,MPI_INT,0,0,MPI_COMM_WORLD);
        }
        // Accessible code block for the p-1 th process
        else {
            int sum_i = 0;
            int *a = malloc(k*sizeof(int));
            MPI_Recv (a,n - (p-1) * k,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            for (int i = 0;i < n - (p-1) * k;i++) {
                sum_i += a[i];
            }
            MPI_Send(&sum_i,1,MPI_INT,0,0,MPI_COMM_WORLD);
        }



    }

    MPI_Finalize();

}
