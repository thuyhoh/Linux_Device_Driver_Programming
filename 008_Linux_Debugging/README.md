# Linux Debugging Using GDB, Valgrind, Strace
## I. GDB
### 1. Installation 
- Install gdb on Debian
``` bash
sudo apt install gdb
```
### 2. Compile a program to use with gdb
- Build
``` bash
gcc -g -o binfile srcfile.c
```
- Include
    - Start debugging
    - Set beakpoint
    - View source code
    - Single-step debugging
    - View variable

### 3. Basic command line in GDB
- load file into gdb
``` bash
gdb ./binaryfile
```
- Load other binfile into gdb
``` bash
# > gdb
file ./binaryfile
```
- Run Program
```bash
# > gdb
run
```
- Quit file
```bash
# > gdb
quit
```
- set beakpoint
``` bash
break <line_numb/funct_name>
```
- delete beakpoint
``` bash
delete <breakpoint_numb>
```
- run next command line
``` bash
next/n
```
- run inside the function body
``` bash
step
```
- back to the top of the function 
``` bash
return 
```
- continue execution untill the next breakpoint or the end of program
``` bash
continue
```
- display variable
``` bash
display <var_name>
print <var_name>
print &<var_name>
ptype <var_name>
```
- set variable
``` bash
set variable <var_name> = <value>
```

### 4. Debugging Coredumped
#### 4.1. The problem
- Lỗi Code Dump/Segmentation là một loại lỗi cụ thể do truy cập bộ nhớ "không thuộc về bạn".
- Core dump là bản sao của bộ nhớ tiến trình 
- tên gọi này bắt nguồn từ thời đại bộ nhớ lõi.
#### 4.2. Fixing Coredumped
``` bash
> ulimit -c unlimited
> gcc -g -o test src_coredumped.c
> ./binfile # generate core file
> cat /proc/sys/kernel/core_pattern
core
> cat /proc/sys/kernel/core_uses_pid
0
> echo "1" > /proc/sys/kernel/core_uses_pid
> echo "/tmp/corefiles/core" > /proc/sys/kernel/core_pattern
```
## II. Valgrind
1. Sử dụng bộ nhớ chưa được khởi tạo
2. Đọc/ghi các khối bộ nhớ đã được giải phóng
3. Đọc/ghi các khối bộ nhớ vượt quá giới hạn cấp phát malloc
4. Đọc/ghi các khối bộ nhớ ngăn xếp không phù hợp
5. Rò rỉ bộ nhớ, con trỏ đến một phần bộ nhớ bị mất vĩnh viễn;
6. Khớp malloc/free hoặc new/delete không chính xác
7. Con trỏ dst và src trong các hàm liên quan memcpy() chồng chéo lên nhau

### 1. Installation 
``` bash
sudo apt install -y valgrind
```
### 2. Compile a program to use with valgrind
``` bash
gcc -o binfile srcfile.c -Wall -ggdb3
```
### 3. Debugging with valgrind
``` bash
valgrind --leak-check=yes ./binfile
```
``` bash
valgrind --tool-memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./binfile
```

## III. Strace
### 1. Installation 
``` bash
sudo apt install -y strace
```
### 2. Debugging with Strace
1. Trace Linux command system calls
``` bash
strace ls -l
```
2. Trace Linux Process PID
``` bash
sudo strace -p PID
```
3. Get summary of Linux Process
``` bash
sudo strace -c -p PID
```
4. Print Instrction Pointer During System call
``` bash
sudo strace -i ls -l
```
5. Show time of day for each trace output line 
``` bash
sudo strace -t ls -l
```
6. Print command Tile spent in system calls
``` bash
sudo strace -T ls -l
```
7. Trace only specific system calls
``` bash
sudo strace -e trace=write ls -l
```
8. Trace System call based cn and Certain condition 
``` bash
sudo strace -q -e trace=process ls -l
sudo strace -q -e trace=file ls -l
sudo strace -q -e trace=memoty ls -l
```
9. Redirect Trace Output to file
``` bash
sudo strace 0o df_debug.txt ls -l
```
10. show Some debugging output of strace
``` bash
strace -d ls -l
```



