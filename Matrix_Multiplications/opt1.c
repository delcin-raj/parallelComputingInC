
// Optimization 1 switching the loops


void continuous_multiply(int *A,int *B,int *C,int n){
    for(int i=0;i<n;i++){
        for(int k=0;k<n;k++){
            for(int j=0;j<n;j++){
                C[i*n+j] += A[i*n+k] * B[k*n+j];
            }
        }
    }
}
