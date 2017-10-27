#include "main_funs.h"

const int filesize = 1024;
const int filenamesize = 128;
const int magic_number = 666;

int arc_write(int fd, char * filename)
{	
/*	char	   buffer[BUFFER_SIZE];
	size_t 	   written_bytes;
	size_t 	   read_bytes;*/
	struct ARC_HEADER Header;
	struct FILE_DATA  Data;


	Header.deleted = 0;
	char * data = data_read(filename);
	//printf("%s\n", data);

	strncpy(Header.name, filename, filenamesize);
	strncpy(Data.data, data, filesize);

	if (is_empty(fd))
	{
		write(fd, &magic_number, sizeof(int));
	}

	lseek(fd, 1, SEEK_END);
	write(fd, &Header, sizeof(struct ARC_HEADER));
	lseek(fd, 1, SEEK_CUR);
	write(fd, &data, sizeof(struct FILE_DATA));

	return 0;
}

/*	int fd1 = open("myarc.stn", O_RDWR | O_CREAT | O_APPEND, 
	   		         		   S_IRWXU | S_IRWXG | S_IRWXO);
	if (fd1 == -1)
	{
		printf("Archive is broken\n");
		return 1;
	};

	int fd2 = open(file, O_RDWR | O_CREAT | O_APPEND, 
	   		         	S_IRWXU | S_IRWXG | S_IRWXO);

	if (fd2 == -1)
	{
		printf("File %s is broken\n", file);
		return 1;
	};



	while ((read_bytes = read (fd2, Data.name, 256)) > 0)
	{		
		written_bytes = write (fd1, Data.name, read_bytes);
		if (written_bytes != read_bytes)
		{
			printf ("Cannot write\n");
			return 1;
		}

	}

	close(fd1);
	close(fd2);

}
*/
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
/*------------------------------------------------------------------------*/
int is_empty(int fd)
{
	off_t size = lseek(fd, 0, SEEK_END);

	return size == 0;
}

int arc_read(int fd, char * filename)
{
	struct FILE_DATA  Data;
	struct ARC_HEADER Header;

	while(read(fd, &Header, sizeof(struct ARC_HEADER))){

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