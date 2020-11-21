#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* The assumption is that |M| << |w| where M is the dfa and 
    w is the input string. M is shared between all threads.
    F is a bit vector F[i] = 1 means i is a accept state else not
*/
#define ALPHABETS 10
#define STATES 5

int M[STATES][ALPHABETS];
int F[STATES];
void * sim_dfa (void *data);
void * initialize (int *text,int str_len);

typedef struct d {
    int span;
    // L represents the current state of the automaton during the processing
    // if it had started from the state 0 (initial state)
    int L;
    int *text;
}thread_data;



int main (int argc,char *argv[]) {
    FILE *fptr = fopen("run_time_single","a");
    clock_t start,end;
    start = clock();
    // data preparation
    int str_len = atoi(argv[argc-1]);
    int *text = (int *) malloc(sizeof(int)*str_len);
    initialize(text,str_len);
    thread_data *data = (thread_data *)malloc(sizeof(thread_data));
    data->text = text; data->span = str_len;

    //simulation
    sim_dfa ((void *)data);

    if (F[data->L] == 1) {
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
    int seed = time(0);
    for (int i=0; i<STATES;i++) {
        for (int j= 0;j<ALPHABETS;j++) {
            // state i on reading a symbol j can go to any other state
            M[i][j] = rand_r(&seed) % STATES;
        }
        // deciding whether i is accepted or not
        F[i] = rand_r(&seed) % 2;
    }
    seed = time(0);
    for (int i=0; i<str_len;i++) {
        text[i] = rand_r(&seed) % ALPHABETS;
    }
}

void * sim_dfa (void *data_) {
    thread_data *data = (thread_data *)data_;
    data->L = 0;
    for (int i=0;i<data->span;i++) {
        data->L = M[data->L][data->text[i]];
    }
}




