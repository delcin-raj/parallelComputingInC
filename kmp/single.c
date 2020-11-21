#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 *  shift of a suffix is defined as the index from which the suffix starts
*/

typedef struct d {
    int *T; // Text array
    long long int start,end; // starting and ending index of the T
    int *P; // pattern array
    int *prefix_func; //prefix function has the same length as P
    int m; // length of P
    int hits; // number of matches
} data_t;

void * compute_prefix_function (int *P,int *prefix_func,int m) {
    // k keeps track of the prefix of P which is a suffix till P[q]
    // such that the shift of the suffix is not 0
    int k = -1; 
    prefix_func[0] = k;
    for (int q=1; q<m; q++) {
        /**
         *  the while loop executes if the there exists a valid prefix of P
         * which is a suffix till P[q-1] but the shift of the suffix is not 0
         * in which case k > -1, and ....
         * that prefix of P is not a valid suffix of till P[q] 
         * in which case P[k+1] != P[q]
         */
        while (k > -1 && P[k+1] != P[q]) {
            k = prefix_func[k];
            // The while loop ends when the valid prefix of P till P[q] 
            // is found. k = -1 implies empty string as the prefix
        }
        if (P[k+1] == P[q]) {
            k = k+1;
        }
        prefix_func[q] = k;
    }
}

void * kmp_matcher (void *data_) {
    data_t *data = (data_t *) data_;
    int q = -1; //initial state
    for (long long int i=data->start; i<=data->end; i++) {
        while (q > -1 && data->P[q+1] != data->T[i]) {
            q = data->prefix_func[q];
        }
        if (data->P[q+1] == data->T[i]) {
            q = q+1;
        }
        /**
         * Due to the while loop and the following if condition
         * the appropriate state in the dfa for the pattern P
         * is reached by the string till T[i]
         */
        if (q == data->m-1) {
            data->hits++;
            // match has occured
            q = data->prefix_func[q];
            // continue the loop and search for the next match
        } 
    }
}

void * initialize(int *P,long long int n) {
    srand(time(0));
    for (int i=0; i<n; i++) {
        P[i] = rand()%4;
    }
}

int main(int argc,char *argv[]) {
    // data preparation
    int m = atoi(argv[argc-2]); // length of the pattern
    long long int n = atoll(argv[argc-1]); // length of the text
    int *P = (int *) malloc(sizeof(int)*m);
    int *prefix_func = (int *) malloc(sizeof(int)*m);
    int *T = (int *) malloc(sizeof(int)*n);
    initialize(P,m);
    initialize(T,n);
    compute_prefix_function(P,prefix_func,m);
    data_t data;
    data.hits = 0; data.start = 0; data.end = n-1; data.m = m;
    data.prefix_func = prefix_func; data.P = P; data.T = T;
    
    FILE *fptr = fopen("run_time_single","a");
    clock_t start,end;

    // I'm only bench marking kmp_matcher because only this function is going 
    // to be parallelized
    start = clock();
    kmp_matcher((void *) &data);
    end = clock();
    double timetaken = (double)(end-start)/CLOCKS_PER_SEC;
    fprintf(fptr,"len(P) = %d len(T) = %lld, timetaken = %f\n",m,n,timetaken);
    printf("%d\n",data.hits);
    fclose(fptr);
    return 0;
}