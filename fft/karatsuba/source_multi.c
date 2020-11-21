#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <pthread.h>

#define MAX_THREADS 6
// At any point except the main thread we want the working thread to be less than
// MAX_THREADS
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int active_threads = 0;

void initialize(double *a,long long int end) {
    for (long long int i=0; i<=end; i++)
        a[i] = 0;
}

void * print_array(double *a,long long int start,long long int end) {
    printf("%lld -> %lld --->",start,end );
    for (long long int i = start; i <= end; i++ ) {
        printf("%0.0f ",a[i] );
    }
    printf("\n");
}

typedef struct data {
    double *a;
    double *b;
    double *c;
    long long int start;
    long long int end;
}thread_data;

void * multiply(void *data_) {
    thread_data *data = (thread_data *) data_;
    long long int start,end;
    start = data->start; end = data->end;
    double *a,*b,*c;
    a = data->a; b = data->b; c = data->c;

    if (start == end) {
        c[0] += a[0]*b[0];
    }
    else {
        // degree describes the range
        long long int degree = end -start;
        thread_data *data_t[3];
        //----------Initialising data for next function call ----------//
        for (int i=0; i<3;i++) {
            data_t[i] = malloc(sizeof(thread_data));
        }
        data_t[0]->a = a;
        data_t[0]->b = b;
        data_t[0]->start = start; data_t[0]->end = start + (degree-1)/2;
        data_t[1]->a = a + (degree + 1)/2;
        data_t[1]->b = b + (degree + 1)/2;
        data_t[1]->start = start+(degree+1)/2; data_t[1]->end = start + degree;
        double *c1,*c2;
        c1 = malloc(degree*sizeof(double));
        c2 = malloc(degree*sizeof(double));
        initialize(c1,degree-1); initialize(c2,degree-1);
        data_t[0]->c = c1;
        data_t[1]->c = c2;

        double *p = malloc(((degree+1)/2)*sizeof(double));
        double *q = malloc(((degree+1)/2)*sizeof(double));
        double *r = malloc(degree*sizeof(double));
        double *a1 = a + (degree + 1)/2;
        double *b1 = b + (degree + 1)/2;
        for (long long int i =0; i < (degree+1)/2; i++) {
            p[i] = q[i] = 0;
            p[i] = *(a+i) + *(a1+i);
            q[i] = *(b+i) + *(b1+i);
        }
        initialize(r,degree-1);
        data_t[2]->a = p; data_t[2]->b = q; data_t[2]->c = r;
        data_t[2]->start = 0; data_t[2]->end = (degree-1)/2;
        //---------------------------------------------------------------//

        // This is where we get parallelism in this context as the results can
        // be written the rsult array without any collision

        /* I'm acquiring locks both to read and write the active_threads variable
            since it is entirely possible for two threads to create two additional
            threads when only one was permissible
        */
        int flag = 0;
        pthread_t t[2];
        pthread_attr_t attr;
        pthread_attr_init (&attr);
        //-----------------------------------------//
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
        //------------------------------------------------------//
        pthread_mutex_lock(&mutex);
        for (int it=0; it<2; it++){
            if (active_threads < MAX_THREADS) {
                active_threads++;
                flag ++;
            }
            else {
                break;
            }
            it++;
        }
        pthread_mutex_unlock(&mutex);
        //------------------------------------------------------//

        for (int it = 0; it<3; it++) {
            if (it < flag) {
                pthread_create(&t[it], &attr, multiply, (void *)data_t[it]);
            }
            else {
                multiply((void *) data_t[it]);
            }
        }
        // The parent thread does the other half
        // Waits for the child
        for (int it = 0; it<2; it++) {
            if (it < flag) {
                pthread_join(t[it],NULL);
            }
            else {
                break;
            }
        }
        for (long long int j = 0; j < degree; j++) {
            c[j] += c1[j];
        }
        for (long long int j = degree+1,k=0; j < degree; j++,k++) {
            c[j] += c2[k];
        }
        for (long long int k=0,i=(degree + 1)/2; k<=degree-1 ;k++,i++) {
            c[i] += r[k] - c1[k] - c2[k] ;
        }
        // The logic is that each thread is allocated this routine and after it's
        // completion they will die and their resources can be used by new threads;
        pthread_mutex_lock(&mutex);
        active_threads--;
        pthread_mutex_unlock(&mutex);
    }
    pthread_mutex_lock(&mutex);
    // print_array(b,start,end);
    // print_array(c,2*start,2*end);
    pthread_mutex_unlock(&mutex);
}

int main(int argc,char *argv[]) {
    FILE *fptr = fopen("multi_t.txt","w");
    long long int degree = (long long int) pow(2,10) - 1;
    for (int j = 1; j <= atoi(argv[argc-1]); j++) {
        clock_t start,end;
        start = clock();

        // At this point only main thread is active
        active_threads = 0;

        double *a = malloc((degree+1)*sizeof(double));
        double *b = malloc((degree+1)*sizeof(double));
        double *c = malloc(((2*degree)+1)*sizeof(double));
        for (long long int k=0; k<=degree;k++) {
            a[k] = b[k] = 1;
        }
        initialize(c,2*degree);
        thread_data *data_t = malloc(sizeof(thread_data));

        //----------------------------------------//
        data_t-> a = a; data_t->b = b; data_t->c = c;
        data_t->start = 0; data_t->end = degree;
        //----------------------------------------//
        multiply((void *)data_t);
        //-----------------------------------------//
        // The result is stored in the array pointed by c
        end = clock();
        double timetaken = (double)(end-start)/CLOCKS_PER_SEC;
        fprintf(fptr, "%lld degree of the resultant polynomial, timetaken = %f\n",2*degree+1,timetaken );
        degree = (degree + 1) * 16 - 1;
        free(a);free(b);free(c);
    }
    pthread_mutex_destroy(&mutex);
    pthread_exit(0);
}
