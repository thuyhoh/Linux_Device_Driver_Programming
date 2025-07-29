#include <stdio.h>

void pr_hello(int i)
{
    printf("hello: %d\n",i);
}

int main(void)
{
    int i;
    printf("Demo GDB\n");
    for (i = 0; i< 5; i++)
    {
        pr_hello(i);
    }
    return 0;
}
