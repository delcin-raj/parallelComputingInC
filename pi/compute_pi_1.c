#include<math.h>
#include<stdio.h>
#include<omp.h>
#include<time.h>
#include<stdlib.h>

// I'm going to use the integral of 4*(1/(1+x^2)) to compute the value of pi

#define NUM_THREADS 4
int main(int argc, char *argv[]){
    FILE *fptr = fopen("result_omp_reduction.txt","w");

    long long int steps = 1000;
    //double sum[NUM_THREADS];
for (int j = 1; j <= atoi(argv[argc-1]); j++) {
    clock_t start,end;
    start = clock();
    omp_set_num_threads(NUM_THREADS);
    steps *= 10;
    double step =1/(double)steps;
      double x;
      long long int i;
      double sum = 0;
      #pragma omp parallel for reduction(+: sum)
      for(i=0;i<=steps;i++){
          x = (i)*step;
          if (i == 0 || i == steps) {
             sum +=  4/(1+(x*x));
	      }
    	  else {
    	      sum += 8/(1+(x*x));
    	  }
      }
    double pi = (step/2)*sum;
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
