#include<math.h>
#include<stdio.h>
#include<omp.h>
#include<time.h>
#include<stdlib.h>

// I'm going to use the integral of 4*(1/(1+x^2)) to compute the value of pi

#define NUM_THREADS 4
int main(int argc, char *argv[]){
    FILE *fptr = fopen("result_partition.txt","w");

    long long int steps = 1000;
    double sum[NUM_THREADS];
for (int j = 1; j <= atoi(argv[argc-1]); j++) {
    clock_t start,end;
    start = clock();
    omp_set_num_threads(NUM_THREADS);
    steps *= 10;
    double step =1/(double)steps;
    long long int factor = steps / NUM_THREADS;
#pragma omp parallel
    {
      double x;
      long long int start,end;
      int ID = omp_get_thread_num();
      start = factor * ID;
      if (ID == NUM_THREADS-1)
        end = steps+1;
      else
        end = start + factor;
      double summ = 0;
      for(;start<end;start++){
          x = (start)*step;
          if (start == 0 || start == steps) {
             summ +=  4/(1+(x*x));
	  }
	  else {
	      summ += 8/(1+(x*x));
	  }
      }
        //#pragma omp critical
	{
            sum[ID] = summ;
	}
    }
    double summ = 0;
    for(int i=0;i<NUM_THREADS;i++)
        summ = summ + sum[i];
    double pi = (step/2)*summ;
    end = clock();
    double timetaken = (double)(end-start)/CLOCKS_PER_SEC;
    double error = M_PI-pi;
    if (error < 0)
        error *= -1;
    printf("%lld %f %f %0.32lf %f\n",steps,step,pi,error,timetaken  );
    fprintf(fptr,"%lld %0.32lf %f\n",steps,error,timetaken );

}
fclose(fptr);
}
