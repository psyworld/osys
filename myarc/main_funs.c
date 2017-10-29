#include "main_funs.h"

const int filesize = 1024;
const int filenamesize = 128;
const int magic_number = 666;

int arc_write(int fd, char * filename)
{	
	struct ARC_HEADER Header;
	struct FILE_DATA  Data;


	Header.deleted = 0;
	char * data = data_read(filename);

	strncpy(Header.name, filename, filenamesize);
	strncpy(Data.data, data, filesize);

	if (is_empty(fd))
	{
		write(fd, &magic_number, sizeof(int));
	}

	write(fd, &Header, sizeof(struct ARC_HEADER));
	write(fd, data, sizeof(struct FILE_DATA));



	return 0;
}
/* for arc_write --------------------------------------------------------*/
char * data_read(char * filename)
{
	int f = open(filename, O_RDONLY);
	if (f == -1)
	{
		printf("%s\n","Fileopen error");
	}

	off_t fsize;

	fsize = lseek(f, 0, SEEK_END);
	char *data = malloc(fsize + 1);
	lseek(f, 0, SEEK_SET);
	read(f, data, fsize);

	data[fsize] = '\0';

	close(f);

	return data;
}

int is_empty(int fd)
{
	off_t size = lseek(fd, 0, SEEK_END);

	return size == 0;
}

/*------------------------------------------------------------------------*/
int arc_read(int fd, char * filename)
{
	struct FILE_DATA  Data;
	struct ARC_HEADER Header;
	off_t read_bytes;

	lseek(fd, sizeof(magic_number), SEEK_SET);
	while((read_bytes = read(fd, &Header, sizeof(struct ARC_HEADER))) > 0){

        read(fd, &Data, sizeof(struct FILE_DATA));

        if (!strcmp(filename, Header.name))
        {
            printf("\nFILENAME: %s\n", Header.name);
            printf("\nDATA:\n%s\n", Data.data);
            return 0;
        }
    }

    return 0;
}