#include <pthread.h>
#include <complex.h>

#define MAX_THREADS 1

typedef struct data_ {
    double *a;
    double complex *A;
    unsigned long int len;
    int id;
}thread_data;
thread_data data1[MAX_THREADS];

void * bit_rev_thread(void *);
unsigned long int rev (unsigned long int,int);

void * bit_reverse(double *a,double complex *A,unsigned long int len) {
    pthread_t p_threads[MAX_THREADS];
    pthread_attr_t attr;
    pthread_attr_init (&attr);
    pthread_attr_setscope (&attr,PTHREAD_SCOPE_SYSTEM);
    for (int i=0; i< MAX_THREADS; i++) {
        data1[i].a = a; data1[i].A = A; data1[i].id = i; data1[i].len = len;
        pthread_create(&p_threads[i],&attr,bit_rev_thread,(void *) &data1[i]);
    }
    // waiting for all the threads to complete to use the result
    for (int i=0; i< MAX_THREADS; i++) {
        pthread_join(p_threads[i],NULL);
    }
}

void * bit_rev_thread(void *data_t) {
    thread_data *data = (thread_data *) data_t;
    for (unsigned long int i = data->id;i < data->len; i += MAX_THREADS) {
        data->A[i] = data->a[rev(i,3)];
    }
}

unsigned long int rev(unsigned long int k,int bit_len) {
    unsigned long int bitstring [bit_len];
    for (int i=0; i<bit_len; i++) {
        bitstring[i] = k%2;
        k /= 2;
    }
    unsigned long int ret = 0,f = 1;
    for (int i=bit_len-1; i>=0; i--) {
        ret += bitstring[i] * f;
        f = f*2;
    }
    return ret;
}
