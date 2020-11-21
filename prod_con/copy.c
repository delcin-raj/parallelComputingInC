#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

#define K 100
#define NUM_PROD 15
#define NUM_CONS 15
#define PROD_COUNT 32
#define CONS_COUNT 32

// condition is defined on the current size of the queue
int size = 0;
int num_jobs = 1342;
int cons_count = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t prod_cond;
pthread_cond_t cons_cond;
pthread_t prod_threads[PROD_COUNT];
pthread_t cons_threads[CONS_COUNT];

typedef struct q_ {
    double queue [K];
    int head,tail;
} Queue;

// The enque or deque is called by producer or consumer respectively
// only if the size of the queue is valid for the operation
void enque(Queue *q, double item) {
    q->queue[q->tail] = item;
    q->tail = (q->tail + 1) % K;
    num_jobs--;
    size++;
}

int deque(Queue *q) {
    int ret = q->queue[q->head];
    q->head = (q->head + 1) % K;
    size--;
    return ret;
}

int prod_finished() {
    int ret;
    if (num_jobs > 0) {
        ret = 0;
    }
    else {
        ret = 1;
    }
    return ret;
}

int cons_finished() {
    int ret;
    if (num_jobs > 0 || size > 0 ) {
        ret = 0;
    }
    else {
        ret = 1;
    }
    return ret;
}


void * producer(void *data_) {
    Queue *q = (Queue *) data_;
    int finished = 0;
    while(!finished) {
        /* Task creation part */
        sleep(1/100);
        pthread_mutex_lock(&mutex);
        if (!prod_finished()) {
            // wait if size == K
            while (size == K) {
                pthread_cond_wait(&prod_cond,&mutex);
            }
            // Because it is possible that 30 threads are waiting in the prod_cond
            // But num_jobs = 10, 20 threads are going to over produce
            if (prod_finished()) {
                pthread_exit(0);
            }
            // If in case this condition is being taken care in task creation part
            // The above checking is not necessary
            enque(q,1/1000);
            printf("Job -> %d\n",num_jobs+1 );
            pthread_cond_broadcast(&cons_cond);
        }
        else {
            finished = 1;
        }
        pthread_mutex_unlock(&mutex);
    }

}

void * consumer (void *data_) {
    Queue *q = (Queue *) data_;
    int finished = 0;
    while(!finished) {
        pthread_mutex_lock(&mutex);
        if (!cons_finished()) {
            // wait if size == 0
            while (size == 0 ) {
                pthread_cond_wait(&cons_cond,&mutex);
            }
            // It is possible that only one task is left in buffer but more than
            // 2 consumer threads are awaken by the condition
            if (cons_finished()) {
                pthread_mutex_unlock(&mutex);
                pthread_exit(0);
            }
            // consuming the task and running it
            sleep(deque(q));
            printf("Consumed->%d\n",++cons_count );
            // possibly some producers are waiting
            if (!prod_finished()) {
                pthread_cond_broadcast(&prod_cond);
            }
        }
        else {
            finished = 1;
        }
        pthread_mutex_unlock(&mutex);
    }
}

int main () {
    Queue *q = malloc(sizeof(Queue));
    q->head = q->tail = 0;
    for (int i=0; i<PROD_COUNT; i++) {
        pthread_create(&prod_threads[i],NULL,producer,(void *)q);
    }
    for (int i=1; i<CONS_COUNT; i++) {
        pthread_create(&cons_threads[i],NULL,consumer,(void *)q);
    }
    for (int i=1; i<CONS_COUNT; i++) {
        pthread_join(prod_threads[i],NULL);
        pthread_join(cons_threads[i],NULL);
    }
    printf("%d\n",num_jobs );
    pthread_exit(0);
}
