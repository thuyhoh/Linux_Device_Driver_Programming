#define _GNU_SOURCE
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/syscall.h>

#define __NR_my_systemcall 451

int main(void)
{
  long ret = syscall(__NR_my_systemcall, 10);
  if (ret < 0) {
    perror("syscall failed"); /* tự đọc errno */
    return 1;
  }
  printf("ret = %ld\n", ret);
  return 0;
}