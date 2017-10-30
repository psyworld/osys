#include "main_funs.h"

int main (int argc, char * argv[])
{
	int fd = open("myarc.stn", O_RDWR | O_CREAT | O_APPEND, 
					S_IRWXU | S_IRWXG | S_IRWXO);
	if (fd == -1)
	{
		printf("%s\n", "Fileopen error 1");
		close(fd);
		return 1;
	}

	arc_write(fd, "test.txt");
	arc_write(fd, "test1.txt");
	arc_list(fd);
	arc_read(fd, "test.txt");
	arc_read(fd, "test1.txt");
	arc_delete(fd, "test.txt");
	arc_read(fd, "test.txt");
	
	close(fd);
	return 0;
}

