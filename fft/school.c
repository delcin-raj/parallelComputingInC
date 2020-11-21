#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define MAX_THREADS 6

typedef struct data {
    double *a;
    double *b;
    double *c;
    long long int degree;
    long long int start;
    long long int end;
}thread_data;
thread_data data[MAX_THREADS];

void* poly_mult (void *data) {
    thread_data *l_data = (thread_data *) data;
    for (long long int i= l_data->start;i <= l_data->end; i++) {
        l_data->c[i] = 0;
        if (i <= l_data->degree) {
            for(long long int j=0; j<=i;j++)
                l_data->c[i] += l_data->a[j]*l_data->b[i-j];

        }
        else {
            for(long long int j=i-l_data->degree; j<=l_data->degree;j++)
                l_data->c[i] += l_data->a[j]*l_data->b[i-j];
        }
    }
    pthread_exit(NULL);
}


int main(int argc,char *argv[]) {
    FILE *fptr = fopen("school_result_6_threads.txt","w");
    long long int degree = 1000;
    for (int j = 1; j <= atoi(argv[argc-1]); j++) {
        clock_t start,end;
        start = clock();

        double *a = malloc((degree+1)*sizeof(double));
        double *b = malloc((degree+1)*sizeof(double));
        double *c = malloc(((2*degree)+1)*sizeof(double));
        for (long long int k=0; k<=degree;k++) {
            a[k] = b[k] = k%100;
        }
        pthread_t p_threads[MAX_THREADS];
        pthread_attr_t attr;
        pthread_attr_init (&attr);
        pthread_attr_setscope (&attr,PTHREAD_SCOPE_SYSTEM);
        // distributing the works among the threads
        for (int i=0; i< MAX_THREADS; i++) {
            data[i].a = a;
            data[i].b = b;
            data[i].c = c;
            data[i].degree = degree;
            if (i < MAX_THREADS-1) {
                data[i].start = i * ((2*degree+1)/MAX_THREADS);
                data[i].end = ((i+1) * ((2*degree+1)/MAX_THREADS)) - 1;
                pthread_create(&p_threads[i],&attr,poly_mult,(void *) &data[i]);
            }
            else {
                data[i].start = i * ((2*degree+1)/MAX_THREADS);
                data[i].end = 2*degree;
                pthread_create(&p_threads[i],NULL,poly_mult,(void *) &data[i]);
            }

        }

        // waiting for all the threads to complete to use the result
        for (int i=0; i< MAX_THREADS; i++) {
            pthread_join(p_threads[i],NULL);
        }
        end = clock();
        double timetaken = (double)(end-start)/CLOCKS_PER_SEC;
        printf("%f\n",timetaken );
        fprintf(fptr, "%lld degree of the resultant polynomial, timetaken = %f\n",2*degree+1,timetaken );
        degree *= 10;
    }
    fclose(fptr);
}
