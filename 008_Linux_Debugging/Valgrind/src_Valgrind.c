#include <stdio.h>
#include <stdlib.h>


int main(void)
{
    char *mem1 = NULL;
    char *mem2 = NULL;

    mem1 = (char*)malloc(sizeof(char) * 4);
    mem2 = (char*)malloc(sizeof(char) * 4);

    printf("Checking memory leak using valgrind tool\n");
    
    free(mem1);
    // free(mem2);  

    return 0;
}


