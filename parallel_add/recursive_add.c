#include "stdio.h"
#include "mpi.h"
#include "stdlib.h"


typedef struct node {
    struct node *adjacent;
    int parent;
    int rank;
    int load; /* To be received from parent */
} Node;

void comm_create(Node *processes,int parent,int child,int load) {
    if (child - parent > 0){
        processes[child].parent = parent;
        processes[child].load = load;
        Node *temp = malloc(sizeof(Node));
        temp->adjacent = NULL;
        temp->rank = child;
        temp->parent = parent;
        Node *y = processes[parent].adjacent;
        if (y == NULL) {
            processes[parent].adjacent = temp;
        } else {
            Node *x;
            while (y != NULL){
                x = y;
                y = y->adjacent;
            }
            x->adjacent = temp;
        }

        comm_create(processes,parent,parent+(child-parent)/2,load/2);
        comm_create(processes,child,child+(child-parent)/2,load/2);
    }
}

void main () {
    MPI_Init(NULL,NULL);
    int p,rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);
    MPI_Comm_size(MPI_COMM_WORLD,&p);

    // Each process has the copy of the communication structure within it
    Node *processes;
    processes = malloc(p*sizeof(Node));
    // Initialising the structure
    for (int i=0;i < p;i++){
        //processes[i] = malloc(sizeof(Node));
        processes[i].adjacent = NULL;
        processes[i].rank = i;
    }
    /* The communication structure is globally visible to all the processes */
    int n =64;
    comm_create(processes,0,p/2,n/2);
    //printf("%d\n",p );
    // verifying communication architecture
    // if (rank == 0) {
    //     for (int i=0;i<p;i++) {
    //         Node *y = processes[i].adjacent;
    //         while (y!=NULL){
    //             printf("%d is the child of process %d\n",y->rank,y->parent);
    //             y = y->adjacent;
    //         }
    //     }
    // }

    if (rank == 0) {
         int sum = 0;
         int array[n];
         for (int i=0;i<n;i++) {
             array[i] = i+1;
         }
         // process 0 has to receive the whole array n
         processes[0].load = n;
         processes[0].parent = -1;
         int load = n;

         // Send the partitioned loads to the children
         Node *y = processes[rank].adjacent;
         while (y != NULL) {
             load = load/2;
             int child = y->rank;
             MPI_Send(array+load,load,MPI_INT,child,0,MPI_COMM_WORLD);
             y = y->adjacent;
             printf("process %d sent load %d to process %d\n",rank,load,child);
         }
         // Doing its job
         for (int i=0;i<n/p;i++) {
             sum += array[i];
         }
         // Receive the sum from it's children
         y = processes[rank].adjacent;
         while (y != NULL) {
             int child = y->rank;
             int sum_i;
             MPI_Recv(&sum_i,1,MPI_INT,child,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
             sum += sum_i;
             y = y->adjacent;
         }
         printf("Sum through MPI %d\n",sum );
         printf("Actual sum is %d\n", (n*(n+1))/2);

     } else {
        int parent = processes[rank].parent;
        int load = processes[rank].load;
        int array[load];
        // Initially receive from its parent
        MPI_Recv(array,load,MPI_INT,parent,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        printf("process %d received load %d from process %d\n",rank,load,parent);
        // Send the loads to its children if exists
        Node *y = processes[rank].adjacent;

        while (y != NULL) {
            load = load/2; /* Only half of the laod is sent off to children */
            int child = y->rank;
            MPI_Send(array+load,load,MPI_INT,child,0,MPI_COMM_WORLD);
            printf("process %d sent load %d to process %d\n",rank,load,child);
            y = y->adjacent;
        }
        int sum = 0;
        // Doing its job
        for (int i=0;i<n/p;i++) {
            sum += array[i];
        }

        // aggregation of result from children if exists
        y = processes[rank].adjacent;
        while (y != NULL) {
            int child = y->rank;
            int sum_i;
            MPI_Recv(&sum_i,1,MPI_INT,MPI_ANY_SOURCE,1,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
            sum += sum_i;
            y = y->adjacent;
        }

        // Send the result back to it's parent for aggregation
        MPI_Send(&sum,1,MPI_INT,parent,1,MPI_COMM_WORLD);
    }
    MPI_Finalize();
}
