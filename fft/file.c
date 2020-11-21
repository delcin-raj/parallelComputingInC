#include<stdio.h>

int main(int argc, char *argv[]){
    FILE *fptr = fopen("result_multi.txt","w");
    fprintf(fptr, "%s\n","whp ha" );
    fclose(fptr);
}
