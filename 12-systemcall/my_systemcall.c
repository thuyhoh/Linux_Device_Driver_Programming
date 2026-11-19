#include <linux/kernel.h>
#include <linux/syscalls.h>

SYSCALL_DEFINE1(my_bbb_ping, int, val)
{
	pr_info("my new system call\n");
	pr_info("my_systemcall called from userspace with val = %d\n", val);
	return val;
}
