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

	if (argc >= 2)
	{
		if (!strcmp(argv[1], "-w"))
		{
			arc_write(fd, argv[2]);
		}
		else if (!strcmp(argv[1], "-r"))
		{
			arc_read(fd, argv[2]);
		}
		else if (!strcmp(argv[1], "-ls"))
		{
			arc_list(fd);
		}
		else if (!strcmp(argv[1], "-cp"))
		{
			arc_copy(fd, argv[2]);
		}
		else if (!strcmp(argv[1], "-rm"))
		{
			arc_delete(fd, argv[2]);
		}
		else if (!strcmp(argv[1], "-mv"))
		{
			arc_rename(fd, argv[2], argv[3]);
		}
	}

	close(fd);
	return 0;
}

