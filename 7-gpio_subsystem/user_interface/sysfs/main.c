#include <stdio.h>
#include <stdlib.h>
#include<string.h> 
#include<unistd.h> 
#include<fcntl.h> 


#define PATH "/sys/class/gpio/"
char file_name[100];

int main(int argc, char *argv[]) 
{
	int i;
	int fd, pin_no;

	if(argc < 2){
        printf("Please enter command-line arguments!\n");
        printf("Usage set output: ./main <pin> <out> <value>\n");
        printf("Usage set input: ./main <pin> <in>\n");
		printf("Usage set unexport: ./main <pin> unexport\n");
        return 1;
	}
	printf("cmd: ");
	for(i = 0; i < argc; i++) {
		printf("%s ", argv[i]);
		if(i == argc-1) printf("\n"); 
	}
	pin_no = atoi(argv[1]);
	if(pin_no == 0 && strcmp(argv[1], "0") != 0) {
		printf("no pin\n");
		return 2;
	}
	printf("setup pin %d\n", pin_no);
	sprintf(file_name, PATH "gpio%d", pin_no);
	if(access(file_name, F_OK) != 0){
		fd = open(PATH "export", O_WRONLY);
		if(fd < 0){
			printf("Error opening /sys/class/gpio/export\n");
			perror(PATH "export");
			return 1;
		}
		write(fd, argv[1], strlen(argv[1]));
		close(fd);
	}   
	memset(file_name, 0, sizeof(file_name));
	sprintf(file_name, PATH "gpio%d/direction", pin_no);
	printf("%s %s\n", file_name, argv[2]);
	fd = open(file_name, O_WRONLY);
	if(fd < 0){
		printf("Error opening %s\n", file_name);
		perror(file_name);
		return 1;
	}
	write(fd, argv[2], strlen(argv[2]));
	close(fd);		

	if(strcmp(argv[2], "out") == 0){
		memset(file_name, 0, sizeof(file_name));
		sprintf(file_name, PATH "gpio%d/value", pin_no);
		fd = open(file_name, O_WRONLY);
		if(fd < 0){
			printf("Error opening %s\n", file_name);
			perror(file_name);
			return 1;
		}
		write(fd, argv[3], strlen(argv[3]));
		close(fd);
	} else if(strcmp(argv[2], "in") == 0){
		memset(file_name, 0, sizeof(file_name));
		sprintf(file_name, PATH "gpio%d/value", pin_no);
		fd = open(file_name, O_RDONLY);
		if(fd < 0){
			printf("Error opening %s\n", file_name);
			perror(file_name);
			return 1;
		}
		char value;
		read(fd, &value, 1);
		printf("Value: %c\n", value);
		close(fd);
	} else if(strcmp(argv[2], "unexport") == 0){
		fd = open(PATH "unexport", O_WRONLY);
		if(fd < 0){
			printf("Error opening /sys/class/gpio/unexport\n");
			perror(PATH "unexport");
			return 1;
		}
		write(fd, argv[1], strlen(argv[1]));
		close(fd);
	} else {
		printf("Invalid argument. Use 'out', 'in' or 'unexport'.\n");
	}
	return 0;
}

