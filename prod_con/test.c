#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

void * func (void * data) {
    printf("%s\n", "wheallo");
}

int main () {
    pthread_t t;
    pthread_create(&t,NULL,func,NULL);
    pthread_exit(0);
}
