#include "main_funs.h"

int main (int argc, char * argv[])
{
	int fd = open("myarc.stn", O_RDWR | O_CREAT, 
					S_IRWXU | S_IRWXG | S_IRWXO);
	if (fd == -1)
	{
		printf("%s\n", "Fileopen error 1");
		close(fd);
		return 1;
	}

	arc_write(fd, "test.txt");
	arc_write(fd, "test1.txt");
	printf("\nAll files:\n");
	arc_list(fd);
	printf("\nFiles data:\n");
	arc_read(fd, "test.txt");
	arc_read(fd, "test1.txt");
	printf("\nDelete one and show list:\n");
	arc_delete(fd, "test.txt");
	arc_read(fd, "test.txt");
	printf("\nAll files:\n");
	arc_list(fd);
	
	close(fd);
	return 0;
}

