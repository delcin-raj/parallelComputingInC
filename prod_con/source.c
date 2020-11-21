#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

FILE *fptr1;
FILE *fptr2;

// condition is defined on the current size of the queue
int size = 0;
int prod_count = 0;
int cons_count = 0;
#define BUFF_SIZE 50
int NUM_JOBS;
double sleep_time;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t prod_cond;
pthread_cond_t cons_cond;

typedef struct q_ {
    double queue [BUFF_SIZE];
    int head,tail;
} Queue;

typedef struct node {
    int id;
    Queue * q;
} thread_data;

// The enque or deque is called by producer or consumer respectively
// only if the size of the queue is valid for the operation
void enque(Queue *q, double item) {
    q->queue[q->tail] = item;
    q->tail = (q->tail + 1) % BUFF_SIZE;
    prod_count++;
    size++;
}

int deque(Queue *q) {
    int ret = q->queue[q->head];
    q->head = (q->head + 1) % BUFF_SIZE;
    size--;
    cons_count++;
    return ret;
}

int prod_finished() {
    int ret;
    if (prod_count < NUM_JOBS) {
        ret = 0;
    }
    else {
        ret = 1;
    }
    return ret;
}

int cons_finished() {
    int ret;
    if (prod_count < NUM_JOBS || size > 0 ) {
        ret = 0;
    }
    else {
        ret = 1;
    }
    return ret;
}


void * producer(void *data_) {
    thread_data *data = (thread_data *)data_;
    Queue *q = data->q; int id = data->id;
    int finished = 0;
    while(!finished) {
        /* Task creation part */
        // It is to be assured by the Job creation part that production is not
        // overdone
        sleep(sleep_time);
        pthread_mutex_lock(&mutex);
        if (!prod_finished()) {
            // wait if size == buff_size
            while (size == BUFF_SIZE) {
                pthread_cond_wait(&prod_cond,&mutex);
            }
            // Because it is possible that 30 threads are waiting in the prod_cond
            // But num_jobs = 10, 20 threads are going to over produce
            if (prod_finished()) {
                pthread_exit(0);
            }
            // If in case this condition is being taken care in task creation part
            // The above checking is not necessary
            enque(q,sleep_time);
            fprintf(fptr2,"Task %d to enter the queue was done by prod_thread %d\n",prod_count,id );
            pthread_cond_broadcast(&cons_cond);
        }
        else {
            finished = 1;
        }
        pthread_mutex_unlock(&mutex);
    }

}

void * consumer (void *data_) {
    thread_data *data = (thread_data *)data_;
    Queue *q = data->q; int id = data->id;
    int finished = 0;
    while(!finished) {
        pthread_mutex_lock(&mutex);
        if (!cons_finished()) {
            // wait if size == 0
            while (size == 0 ) {
                if (!prod_finished()) {
                    pthread_cond_wait(&cons_cond,&mutex);
                }
                else {
                    pthread_mutex_unlock(&mutex);
                    pthread_exit(0);
                }
            }
            // consuming the task and running it
            sleep(deque(q));
            fprintf(fptr2,"Task %d to leave the queue was consumed by cons_thread %d \n",cons_count,id );
            // possibly some producers are waiting
            pthread_cond_broadcast(&prod_cond);
        }
        else {
            finished = 1;
        }
        pthread_mutex_unlock(&mutex);
    }
}

int main (int argc,char *argv[]) {
    fptr1 = fopen("analysis.txt","a");
    fptr2 = fopen("correctness.txt","w");

    // Getting input from commandline
    int num_prod,num_cons;
    NUM_JOBS = atoi(argv[argc-1]);
    num_prod = atoi(argv[argc-2]);
    num_cons = atoi(argv[argc-3]);
    sleep_time = atof(argv[argc-4]);
    //--------------------------------//

    clock_t start,end;
    start = clock();
    Queue *q = malloc(sizeof(Queue));
    q->head = q->tail = 0;
    pthread_t prod_threads[num_prod];
    pthread_t cons_threads[num_cons];
    thread_data prod_data[num_prod];
    thread_data cons_data[num_cons];

    //-------------------------------//

    for (int i=0; i<num_prod; i++) {
        prod_data[i].q = q; prod_data[i].id = i;
        pthread_create(&prod_threads[i],NULL,producer,(void *)&prod_data[i]);
    }
    for (int i=0; i<num_cons; i++) {
        cons_data[i].q = q; cons_data[i].id = i;
        pthread_create(&cons_threads[i],NULL,consumer,(void *)&cons_data[i]);
    }
    for (int i=0; i<num_prod; i++) {
        pthread_join(prod_threads[i],NULL);
    }
    for (int i=0; i<num_cons; i++) {
        pthread_join(cons_threads[i],NULL);
    }
    end = clock();
    double timetaken = (double)(end-start)/CLOCKS_PER_SEC;
    double throughput = timetaken / (num_prod + num_cons);
    fprintf(fptr1,"->%d,%d num of producer and consumer threads used with %d buff_size and %d jobs with sleep time %f\n",
            num_prod,num_cons,BUFF_SIZE,NUM_JOBS,sleep_time);
    fprintf(fptr1,"->Throughput is %lf \n\n", throughput);
    pthread_exit(0);
}
