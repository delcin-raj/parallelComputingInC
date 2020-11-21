#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>


void * print_array(double *a,long long int start,long long int end) {
    printf("%lld -> %lld --->",start,end );
    for (long long int i = start; i <= end; i++ ) {
        printf("%0.0f ",a[i] );
    }
    printf("\n");
}

void * multiply(double *p,double *q,double *r,long long int start,long long int end) {
    if (start == end) {
        r[2*start] += p[0]*q[0];
    }
    else {
        // degree describes the range
        long long int degree = (end -start);
        double *p0 = p;
        double *p1 = p + (degree + 1)/2;
        double *q0 = q;
        double *q1 = q + (degree + 1)/2;
        multiply(p0, q0, r, start, start + (degree-1)/2);
        multiply(p1, q1, r, start+((degree+1)/2), start+degree);
        double *a = malloc(((degree+1)/2)*sizeof(double));
        double *b = malloc(((degree+1)/2)*sizeof(double));
        double *c = malloc(degree*sizeof(double));
        for (long long int i =0; i < (degree+1)/2; i++) {
            a[i] = b[i] = 0;
            a[i] = *(p0+i) + *(p1+i);
            b[i] = *(q0+i) + *(q1+i);
        }
        for (long long int i =0; i < (degree); i++)
            c[i] = 0;
        multiply(a, b , c, 0, (degree-1)/2);
        // Copying the first half result, the degree of that polynomial is degree - 1
        double *temp = malloc(degree*sizeof(double));

        for (long long int i=0; i<degree; i++) {
            temp[i] = r[2*start+i];
        }
        for (long long int i=2*start+(degree + 1)/2,k=0,j=2*start+(degree+1); k<=degree-1 ; i++,k++,j++) {
            r[i] += c[k] - temp[k] - r[j];
        }

        free(a);free(b);free(c);free(temp);
    }
    // print_array(r,2*start,2*end);
    print_array(p,start,end);
}

int main(int argc,char *argv[]) {
    FILE *fptr = fopen("single_t.txt","w");
    long long int degree = 3;
    for (int j = 1; j <= atoi(argv[argc-1]); j++) {
        clock_t start,end;
        start = clock();

        double *a = malloc((degree+1)*sizeof(double));
        double *b = malloc((degree+1)*sizeof(double));
        double *c = malloc(((2*degree)+1)*sizeof(double));
        for (long long int k=0; k<=degree;k++) {
            a[k] = b[k] = 1;
            c[k] = c[k+degree] = 0;
        }
        multiply(a,b,c,0,degree);
        end = clock();
        double timetaken = (double)(end-start)/CLOCKS_PER_SEC;
        printf("%f\n",timetaken );
        print_array(c,0,2*degree);
        fprintf(fptr, "%lld degree of the resultant polynomial, timetaken = %f\n",2*degree+1,timetaken );
        degree = (degree + 1) * 16 - 1;
        free(a);free(b);free(c);
    }
}
