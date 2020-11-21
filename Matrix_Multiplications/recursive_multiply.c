void recursive_multiply(int *A,int *B, int*C,int row_a,int col_a,int row_b,int col_b,int row_c,int col_c,int n,int size){
    if(size==1){
        C[row_c*n+col_c] += A[row_a*n+col_a]*B[row_b*n+col_b];
    }
    else{
        /* A = [A1 A2] B = [B1 B2] C = [A1*B1+A2*B3  A1*B2+A2*B4]
               [A3 A4]     [B3 B4]     [A3*B1+A4*B3  A3*B2+A4*B4]
        */
        
        // A1*B1
        recursive_multiply(A,B,C,row_a,col_a,row_b,col_b,row_c,col_c,n,size/2);
        // A1*B2
        recursive_multiply(A,B,C,row_a,col_a,row_b,col_b+size/2,row_c,col_c+size/2,n,size/2);
        // A2*B3
        recursive_multiply(A,B,C,row_a,col_a+size/2,row_b+size/2,col_b,row_c,col_c,n,size/2);
        // A2*B4
        recursive_multiply(A,B,C,row_a,col_a+size/2,row_b+size/2,col_b+size/2,row_c,col_c+size/2,n,size/2);
        
        // A4*B3
        recursive_multiply(A,B,C,row_a+size/2,col_a+size/2,row_b+size/2,col_b,row_c+size/2,col_c,n,size/2);
        // A4*B4
        recursive_multiply(A,B,C,row_a+size/2,col_a+size/2,row_b+size/2,col_b+size/2,row_c+size/2,col_c+size/2,n,size/2);
        // A3*B1
        recursive_multiply(A,B,C,row_a+size/2,col_a,row_b,col_b,row_c+size/2,col_c,n,size/2);
        // A3*B2
        recursive_multiply(A,B,C,row_a+size/2,col_a,row_b,col_b+size/2,row_c+size/2,col_c+size/2,n,size/2);
    }
}
    

