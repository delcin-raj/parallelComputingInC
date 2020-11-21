#include<stdio.h>
#include<stdlib.h>
int main() {
    int *p[4];
    for (int i=0;i<4;i++)
        p[i] = malloc(sizeof(int));
    printf("%0.0f",(double)0);
}
