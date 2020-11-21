#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "functions.h"
#include <complex.h>


void * print_array(double complex*a,long long int start,long long int end) {
    printf("%lld -> %lld --->",start,end);
    for (long long int i = start; i <= end; i++ ) {
        printf ("%0.1f + i%0.1f ",creal(a[i]),cimag(a[i])) ;
    }
    printf("\n");
}

int my_log (unsigned long int k) {
    // it is assumed that k is a power of 2
    int count = 0;
    while (k > 1) {
        count ++;
        k = k/2;
    }
    return count;
}

int main(int argc,char *argv[]) {
    FILE *fptr = fopen("fft_single.txt","w");
    unsigned long int n = 64 ;
    for (int j = 1; j <= atoi(argv[argc-1]); j++) {
        clock_t start,end;
        start = clock();
        double *a = (double *) malloc(2*n*sizeof(double));
        double *b = (double *) malloc(2*n*sizeof(double));
        double complex *B = (double complex*) malloc(2*n*sizeof(double));
        double complex *A = (double complex*) malloc(2*n*sizeof(double));
        double complex *C = (double complex*) malloc(2*n*sizeof(double));
        for (int i = 0; i< 2*n; i++) {
            if (i < n){
                a[i] = 1; b[i] = 1;
            }
            else {
                a[i] = 0; b[i] = 0;
            }
        }
        int bit_len = my_log(n);
        bit_reverse(a,A,bit_len);
        bit_reverse(b,B,bit_len);
        fft(A,2*n,1,bit_len);
        fft(B,2*n,1,bit_len);
        for (int i = 0; i< 2*n; i++) {
            C[i] = A[i]*B[i];
        }
        fft(C,2*n,-1,bit_len);
    }
}
