#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
int main(){
	FILE *fp_proc, *fp_dev;
	char buffer[1024], output[30];
	size_t bytes_read;
	char *match;
	int i;
	unsigned long ioctlcmd;
		//First, Open the proc file to read the content of it
	fp_proc = fopen("/proc/plog", "r");
	if(!fp_proc){
		printf("/proc/plog is not available\n");
		return 0;
		}

	//Second, By fread, we will obtain the whole contnet which is tuples of IOCTL_SYSNAME//commands name and value
	bytes_read = fread(buffer, 1, sizeof(buffer), fp_proc);
	fclose(fp_proc);

	if(bytes_read == 0 || bytes_read == sizeof(buffer)){
		printf("Some unexpected error in read process\n");
		return 0;
		}


	printf("--------   Procfs-IOCTL System Information   --------\n");
	printf("Enter a Number Between 1 to 3:\n");
	printf("1:Reset All\t2:Reset Stack\n3:Reset Fifo\n\n");
	printf("Your Choice: ");
	scanf("%d", &i);
	printf("-----------------------------------------------------\n");
	switch(i){
		case 1:
			match = strstr(buffer, "IOCTL_RESET");
			if(!match){
				printf("IOCTL_RESET has not found\n");
				return 0;
				}

			sscanf(match, "IOCTL_RESET, %lu", &ioctlcmd);
			break;
		case 2:
			match = strstr(buffer, "IOCTL_STACKRESET");
			if(!match){
				printf("IOCTL_STACKRESET has not found\n");
				return 0;
				}

			sscanf(match, "IOCTL_STACKRESET, %lu", &ioctlcmd);
			break;
		case 3:
			match = strstr(buffer, "IOCTL_FIFORESET");
			if(!match){
				printf("IOCTL_FIFORESET has not found\n");
				return 0;
				}

			sscanf(match, "IOCTL_FIFORESET, %lu", &ioctlcmd);
			break;
		default:
			printf("Invalid Command\n");
			return -1;
	}
	
	int fd = open("/proc/plog", O_RDONLY);

	ioctl(fd, ioctlcmd, output);
	close(fd);

	printf("Output: %s\n", output);
	return 0;
}
