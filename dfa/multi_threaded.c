#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

/* The assumption is that |M| << |w| where M is the dfa and 
    w is the input string. M is shared between all threads.
    F is a bit vector F[i] = 1 means i is a accept state else not
*/
#define ALPHABETS 10
#define STATES 5
#define MAX_THREADS 6


int M[STATES][ALPHABETS];
int F[STATES];
void * sim_dfa (void *data);
void * initialize_t (void *data);
void * initialize (int *text,int str_len);

typedef struct d {
    int id,start,end;
    // L[i] represents the current state of the automaton during the processing
    // if it had started from the state i 
    int L[STATES];
    int *text;
}thread_data;

typedef struct d1 {
    int start,end;
    int *text;
}thread_data1;



int main (int argc,char *argv[]) {
    FILE *fptr = fopen("run_time_multi","a");
    clock_t start,end;
    start = clock();
    // data preparation
    int str_len = atoi(argv[argc-1]);
    int *text = (int *) malloc(sizeof(int)*str_len);
    initialize(text,str_len);
    thread_data data[MAX_THREADS];
    pthread_t threads[MAX_THREADS];
    int span = str_len/MAX_THREADS;
    for (int i=0; i<MAX_THREADS;i++) {
        data[i].id = i;
        if (i == MAX_THREADS-1) {
            data[i].end = str_len; 
        }
        else {
            data[i].end = (i+1)*span; 
        }
        data[i].start = i*span;
        data[i].text = text;
        //simulating the dfa parallelly
        pthread_create (&threads[i],NULL,sim_dfa,(void *)&data[i]);
    }

    for (int i=0; i<MAX_THREADS; i++) {
        pthread_join(threads[i],NULL);
    }
    // Accumulating the results sequentially
    int cur_state = data[0].L[0];
    for (int i=1; i<MAX_THREADS; i++) {
        // L[cur_state] is the state the automaton would be if it had 
        // started at cur_state and processed the part of text allocated
        // to thread i
        cur_state = data[i].L[cur_state];
    }
    if (F[cur_state] == 1) {
        printf("The string is accepted\n");
    }
    else {
        printf("The string is not accepted\n");
    }
    end = clock();
    double timetaken = (double)(end-start)/CLOCKS_PER_SEC;
    fprintf(fptr,"%d states, %d alphabets, %d length of the input,%f time taken\n",STATES,ALPHABETS,str_len,timetaken);    
    return 0;
}

void * initialize (int *text,int str_len) {
    // parallely initializing the input string text
    // pthread_t threads[MAX_THREADS];
    // thread_data1 data[MAX_THREADS];
    // int span = str_len/MAX_THREADS;
    // for (int i=0; i<MAX_THREADS; i++) {
    //     if (i == MAX_THREADS-1) {
    //         data[i].end = str_len; 
    //     }
    //     else {
    //         data[i].end = (i+1)*span; 
    //     }
    //     data[i].start = i*span;
    //     data[i].text = text;
    //     //simulating the dfa parallelly
    //     pthread_create (&threads[i],NULL,initialize_t,(void *)&data[i]);
    // }

    thread_data1 data;
    data.start = 0; data.end = str_len; data.text = text; 
    initialize_t((void *)&data);

    // the main thread initializes finite state automaton M
    int seed = time(0);
    for (int i=0; i<STATES;i++) {
        seed = time(0);
        for (int j= 0;j<ALPHABETS;j++) {
            // state i on reading a symbol j can go to any other state
            M[i][j] = rand_r(&seed) % STATES;
        }
        // deciding whether i is accepted or not
        F[i] = rand_r(&seed) % 2;
    }

    // for (int i=0; i<MAX_THREADS; i++) {
    //     pthread_join(threads[i],NULL);
    // }
}

void * initialize_t(void *data_) {
    thread_data1 *data = (thread_data1 *)data_;
    int seed = time(0);
    for (int i=data->start; i<data->end; i++) {
        data->text[i] = rand_r(&seed) % ALPHABETS;
        seed *= i;
    }
}

void * sim_dfa (void *data_) {
    thread_data *data = (thread_data *)data_;
    for (int i=0; i < STATES; i++) {
        data->L[i] = i;
        if (data->id == 0) {
            break;
        }
    }
    for (int i=0; i<STATES; i++) {
        for (int j=data->start;j<data->end;j++) {
            data->L[i] = M[data->L[i]][data->text[j]];
        }
        if (data->id == 0) {
            break;
        }
    }
}