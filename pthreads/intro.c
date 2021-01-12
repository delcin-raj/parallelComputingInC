/* POINTS TO REMEMBER
* List of abstractions
* CPU - Only the assembly lang designer has to worry about it's implementation
* process and threads - given by os
* process/user threads are mapped to kernel threads for execution
* kernel threads are mapped to CPU for computation
* ALWAYS BEWARE OF PREEMPTIVE SCHEDULING
* Shared memory leads to race condition
* Locking is one of the solution
* POSIX is a threading library which provides an interface to create user threads
* In PSOIX there exists one to one mapping between user threads and kernel threads
*/

#include<stdio.h>
#include<pthread.h>

// All global variables are shared by all the threads
double sum = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* unsafe_add(void* arg) {
	for(int i = 0; i < 10000; i++) {
		sum++;
	}
}

void* safe_add(void* arg) {
	for(int i = 0; i<10000;i++) {
		pthread_mutex_lock(&mutex);
		sum++;
		pthread_mutex_unlock(&mutex);
	}
}	

int main() {
	// We create two threads
	pthread_t t1,t2;
	// function signature for pthread_create
	// int pthread_create(pthread_t *thread, const pthread_attr_t *attr,void *(*start_routine) (void *), void *arg);
	pthread_create(&t1,NULL,unsafe_add,NULL);
        pthread_create(&t2,NULL,unsafe_add,NULL);
	// wait for completion
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
	// wrong result due to race condition
	printf("sum with race condition %f\n",sum);
	
	// correct solution
	sum = 0; // resetting the value
	pthread_create(&t1,NULL,safe_add,NULL);
	pthread_create(&t2,NULL,safe_add,NULL);
	// wait for completion
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
	printf("safe sum %f\n",sum);
	// notice the fact that the functions add1000 and safe_add are called
	// inside the thread stack by the help of pthread interface
	// we are not calling it explicitly.
	// If not for pthread interface we have to call the kernel functions
	// directly to use kernel threads
}
