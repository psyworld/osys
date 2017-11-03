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

	printf("\nAll files:\n");
	arc_list(fd);
	
	printf("\nAdd one file and see list:\n");
	arc_write(fd, "test3.txt");
	arc_list(fd);
	arc_read(fd, "test3.txt");

	printf("\nMake copy\n");
	arc_copy(fd, "test3.txt");
	arc_list(fd);
	arc_read(fd, "test3.txt(copy)");
	
	printf("\nRename test1.txt into _test1.txt\n");

	arc_rename(fd, "test1.txt", "_test1.txt");
	arc_list(fd);

	close(fd);
	return 0;
}

