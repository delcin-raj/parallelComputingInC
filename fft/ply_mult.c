#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


int main(int argc,char *argv[]) {
    int n = 7; // degree of the polynomial
    int a[n+1];
    int b[n+1];
    int c[2*n+1];
    printf("Enter a\n");
    for(int i=0; i<=n; i++)
        scanf("%d",a+i);
    printf("Enter b\n");
    for(int i=0; i<=n; i++)
        scanf("%d",b+i);
    for(int i=0; i<2*n+1; i++) {
        c[i] = 0;
        if (i <= n) {
            for(int j=0; j<=i;j++)
                c[i] += a[j]*b[i-j];
        }
        else {
            for(int j=i-n; j<=n;j++)
                c[i] += a[j]*b[i-j];
        }
    }
    for (int i=0; i<2*n+1; i++) {
        printf("%d ",c[i]);
    }
}
