#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main(void)
{
    char read_buff[1024];
    char write_buff[1024] = "Debug with Strace\n";

    int fd = open("text.txt", O_RDWR);

    if(fd < 0)
    {
        printf("Can not open file ...\n");
        return 0;
    }

    write(fd, write_buff, strlen(write_buff)+1);
    read(fd, read_buff, 1024);

    printf("Data = %s", read_buff);
    close(fd);

    return 0;
}

