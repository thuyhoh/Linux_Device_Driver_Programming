#include <stdio.h>

void foo(void)
{
    int *p = NULL;
    printf("*p = %d\n", *p);
    free(p);
}

int main(void)
{
    printf("Demo Core Dumped\n");
    
    foo();

    return 0;
}

