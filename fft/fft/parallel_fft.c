#include <pthread.h>
#include <complex.h>
#include <math.h>
#define MAX_THREADS 6

unsigned long int m;
double complex w_m;

typedef struct data {
    double complex*A;
    unsigned long int n;
    int id;
}thread_data;
thread_data data[MAX_THREADS];

void * pfft (void * data_) {
    thread_data * data = (thread_data *) data_;
    for (unsigned long int k = data->id; k < (((data->n)/m)-1); k += MAX_THREADS) {
        double complex w = 1,t,u;
        for (int j=0; j <= m/2 -1; j++) {
            t = w * data->A[k+j+m/2];
            u = data->A[k+j];
            data->A[k+j] = u+t;
            data->A[k+j+m/2] = u - t;
            w = w*w_m;
        }
    }
}

void fft (double complex *A, unsigned long int len,int inv,int bit_len) {
    for (int s = 1; s <= bit_len; s++) {
        m = (unsigned long int) pow(2,s);
        w_m = inv * cexp((I * 2 * M_PI)/m);
        pthread_t p_threads[MAX_THREADS];
        pthread_attr_t attr;
        pthread_attr_init (&attr);
        pthread_attr_setscope (&attr,PTHREAD_SCOPE_SYSTEM);
        for (int i = 0; i < MAX_THREADS && i< len/m - 1; i++) {
            data[i].A = A; data[i].n = len; data[i].id = i;
            pthread_create(&p_threads[i],&attr,pfft,(void *) &data[i]);
        }
        for (int i=0; i< MAX_THREADS && i< len/m -1; i++) {
            pthread_join(p_threads[i],NULL);
        }
    }
    if (inv == -1) {
        for (int i = 0; i< len; i++) {
            A[i] /= len;
        }
    }
}
