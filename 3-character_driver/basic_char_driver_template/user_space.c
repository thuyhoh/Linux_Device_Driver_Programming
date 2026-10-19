#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define DEVICE_PATH "/dev/my_char_device"
#define BUFFER_SIZE 256

int main (int argc, char *argv[])
{
    int fd;
    char write_buf[BUFFER_SIZE] = "Hello from User-space application!";
    char read_buf[BUFFER_SIZE];
    ssize_t bytes_written, bytes_read;
    
    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        printf("error: open() %s failed\n", DEVICE_PATH);
        return errno;
    }

    bytes_written = write(fd, write_buf, strlen(write_buf));
    if (bytes_written < 0) {
        printf("error: write() failed\n");
        close(fd);
        return errno;  
    }

    memset(read_buf, 0, BUFFER_SIZE);
    bytes_read = read(fd, read_buf, BUFFER_SIZE - 1);
    if (bytes_read < 0) {
        printf("error: read() failed\n");
        close(fd);
        return errno;  
    }

    if (close(fd) < 0) {
        printf("error: close() failed\n");
        return errno;  
    }

    return 0;
}

