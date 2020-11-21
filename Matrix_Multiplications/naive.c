#include "stdio.h"
#include "stdlib.h"
#include "time.h"

void naive_multiply(int *A,int *B,int *C,int n){
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            for(int k=0;k<n;k++){
                C[i*n+j] += A[i*n+k] * B[k*n+j];
            }
        }
    }
}





int main(){
    srand(time(0));
    int n;
    printf("Enter the dimenson\n");
    scanf("%d",&n);
    int *A,*B,*C;
    A = (int *) malloc (n*n*sizeof(int));
    B = (int *) malloc (n*n*sizeof(int));
    C = (int *) malloc (n*n*sizeof(int));
    
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            A[n*i+j] = rand();
            B[i*n+j] = rand();
            C[n*i+j] = 0;
        }
    }
    clock_t start=clock();
    recursive_multiply(A,B,C,0,0,0,0,0,0,n,n);
    //continuous_multiply(A,B,C,n);
    //naive_multiply(A,B,C,n);
    clock_t end = clock();
    double time = ((double)(end-start))/CLOCKS_PER_SEC;
    printf("%f \n",time);
    /*
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            printf("%d  ",C[i*n+j]);
        }
        printf("\n");
    }
    */
}
    
    
    
    
    
    
    
    
    
