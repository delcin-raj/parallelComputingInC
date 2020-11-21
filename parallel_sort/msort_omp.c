#ifdef _OPENMP
    #include<omp.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void p_ms(double *a,int left,int right);
void p_merge(double *a,int left,int middle,int right);
int b_s(double *a,int left,int right,double key);

int main () {
    double *a;
    int n;
    srand(time(0));
    scanf("%d",&n);
    if (( a = (double *) malloc(sizeof(double) * (n))) == NULL)
        perror("memory allocation failure for L");
    for (int i=0;i<n;i++)
        a[i] = rand()%100;
    for (int i=0;i<n;i++)
        printf("%lf ",a[i] );
    printf("\n--------------------------------------\n");
    p_ms(a,0,n-1);
    for (int i=0;i<n;i++)
        printf("%lf ",a[i] );
    printf("\n");
}
void p_ms(double *a,int left,int right) {
    int middle = (left + right) / 2;
    if (left < right) {
        #pragma omp parallel sections
        {
            #pragma omp section
                p_ms(a,left,middle);
            #pragma omp section
                p_ms(a,middle+1,right);
        }
    }
    p_merge(a,left,middle,right);
}

void p_merge(double *a,int left,int middle,int right) {
    if (left < right && left <= middle) {
        double *L,*R;
        if (( L = (double *) malloc(sizeof(double) * (middle-left+1))) == NULL)
            perror("memory allocation failure for L");
        if (( R = (double *) malloc(sizeof(double) * (right-middle))) == NULL)
            perror("memory allocation failure for R");

        int i;
        #pragma omp parallel for shared(a,left,middle,right) \
                private(i)
            for (i=left; i<=right; i++){
                if (i <= middle) {
                    L[i-left] = a[i];
                }
                else {
                    R[i-middle-1] = a[i];
                }
        }
        int l_m = (middle - left) / 2;
        double key = L[l_m];
        int pivot = b_s(R,0,right-middle-1,key);
        if (pivot == -1) {
            p_merge(a,left+l_m+1,middle,right); // (3/4)n
        }else {
            #pragma omp parallel for shared(a,L,R,l_m,pivot,key) \
                    private(i)
                for (i=l_m+left; i<=right;i++) {
                    if (i<=l_m+left+pivot)
                        a[i] = R[i-l_m-left];
                    else if (i == left+l_m+pivot+1)
                        a[i] = key;
                    else if (left+l_m+pivot+2 <= i && i <= middle+pivot+1)
                        a[i] = L[i-pivot-left-1];
                    else
                        a[i] = R[i-middle-1];
            }
            #pragma omp parallel sections
            {
                #pragma omp section
                    if (pivot < right-middle-1) // (3/4)n
                        p_merge(a,left+l_m+pivot+2,middle+pivot+1,right);
                #pragma omp section
                    p_merge(a,left,left+l_m-1,left+l_m+pivot);
            }

        }

    }
}

int b_s(double *a,int left,int right,double key) {
    int middle = (left+right) / 2;
    if (left > right)
        return right;
    if (a[middle] == key)
        return middle;
    else if (a[middle] <= key)
        return b_s(a,middle+1,right,key);
    else
        return b_s(a,left,middle-1,key);
}
