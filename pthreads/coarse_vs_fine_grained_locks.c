#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<time.h>

#define M 100000
#define N 10
#define I 100000

pthread_mutex_t mutexes[N];
pthread_mutex_t coarse_mutex = PTHREAD_MUTEX_INITIALIZER;
int array[M];

void init() {
	for (int i = 0; i < N; i++) {
		pthread_mutex_init(mutexes+i,NULL);
	}
}

void clear() {
	for (int i = 0; i < M; i++) {
		array[i] = 0;
	}
}
int sum() {
	int res = 0;
	for (int i = 0; i < M;i++) {
		res += array[i];
	}
	// Expected result is 100000
	return res;
}

void *random_increment_coarse_locked(void* arg) {
	int i;
	for (int k = 0; k < I; k++) {
		// Choose an index at random and increment it for 10000 times
		i = rand() % M;
		pthread_mutex_lock(&coarse_mutex);
		array[i]++;
		pthread_mutex_unlock(&coarse_mutex);
	}
}

void *random_increment_fine_locked(void* arg) {
	int i;
	for (int k = 0; k < I; k++) {
		// Choose an index at random and increment it for 10000 times
		i = rand() % M;
		pthread_mutex_lock(&mutexes[i % N]);
		array[i]++;
		pthread_mutex_unlock(&mutexes[i % N]);
	}
}

int main() {
	clock_t start,time_taken1,time_taken2;
	srand(time(0));
	// declaration of array of 10 threads
	pthread_t ts[N];

	// initializing mutexes
	init();

	// Creation of 10 threads
	start = clock();
	for (int i = 0; i < 10; i++) {
		pthread_create(ts+i,NULL,random_increment_coarse_locked,NULL);
	}

	// Wait for the threads to finish
	for (int i=0; i < 10; i++) {
		pthread_join(ts[i],NULL);
	}
	printf("The result is %d\n",sum());
	time_taken1 = clock() - start;
	printf("time taken for coarsed locking scheme %ld \n",time_taken1);

	// Clear the array
	clear();

	// Repeat the same experiment with fine grained locks
	start = clock();
	// Creation of 10 threads
	for (int i = 0; i < 10; i++) {
		pthread_create(ts+i,NULL,random_increment_fine_locked,NULL);
	}

	// Wait for the threads to finish
	for (int i=0; i < 10; i++) {
		pthread_join(ts[i],NULL);
	}
	printf("The result is %d\n",sum());
	time_taken1 = clock() - start;
	printf("time taken for fine grained locking scheme %ld \n",time_taken2);
	printf("----------------------\n");
	printf("Speed UP is %ld\n",time_taken1/time_taken2);

}

