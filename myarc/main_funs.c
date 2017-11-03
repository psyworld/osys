#include "main_funs.h"

const int filesize 	   = 1024;
const int filenamesize = 128;
const int magic_number = 666;

int arc_write(int fd, char * filename)
{	
	struct ARC_HEADER Header;
	struct FILE_DATA  Data;


	Header.deleted = 0;
	char *    data = data_read(filename);

	strncpy(Header.name, filename, filenamesize);
	strncpy(Data.data, data, filesize);

	if (is_empty(fd))
	{
		write(fd, &magic_number, sizeof(int));
	}

	if (exist(fd, filename))
	{
		printf("%s\n", "File already exists");
		return 0;
	}

	del_files(fd);
	write(fd, &Header, sizeof(struct ARC_HEADER));
	write(fd, data,    sizeof(struct FILE_DATA));

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

	off_t fsize = lseek(f, 0, SEEK_END);
	char *data  = malloc(fsize + 1);
	lseek(f, 0, SEEK_SET);
	read(f, data, fsize);

	data[fsize] = '\0';

	close(f);

	return data;
}

int is_empty(int fd)
{
	off_t size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

	return size == 0;
}

int exist(int fd, char * filename)
{
	struct FILE_DATA  Data;
	struct ARC_HEADER Header;	
	off_t  read_bytes;

	lseek(fd, sizeof(magic_number), SEEK_SET);
	while((read_bytes = read(fd, &Header, sizeof(struct ARC_HEADER))) > 0)
	{

        read(fd, &Data, sizeof(struct FILE_DATA));

        if (!strcmp(filename, Header.name) && Header.deleted == 0) 
        {
            lseek(fd, 0, SEEK_SET);

        	return 1;
        }
    }

    return 0;
}

int del_files(int fd) //point to deleted file
{
	struct FILE_DATA  Data;
	struct ARC_HEADER Header;
	off_t  read_bytes;

	lseek(fd, sizeof(magic_number), SEEK_SET);
	while((read_bytes = read(fd, &Header, sizeof(struct ARC_HEADER))) > 0)
	{
        if (Header.deleted)
        {
           	off_t offset = lseek(fd, 0, SEEK_CUR);
           	lseek(fd, offset-sizeof(struct ARC_HEADER), SEEK_SET);

            return fd;
        }
        read(fd, &Data, sizeof(struct FILE_DATA));
    }

	//lseek(fd, sizeof(magic_number), SEEK_SET);

    return 0;

}
/*------------------------------------------------------------------------*/
int arc_read(int fd, char * filename)
{
	struct FILE_DATA  Data;
	struct ARC_HEADER Header;
	off_t  read_bytes;

	lseek(fd, sizeof(magic_number), SEEK_SET);
	while((read_bytes = read(fd, &Header, sizeof(struct ARC_HEADER))) > 0)
	{

        read(fd, &Data, sizeof(struct FILE_DATA));

        if (!strcmp(filename, Header.name) && !(Header.deleted))
        {
            printf("\nFilename: %s\n", Header.name);
            printf("Data:\n%s\n", Data.data);
            //printf("deleted %d\n", Header.deleted);

            lseek(fd, 0, SEEK_SET);

            return 0;
        }
    }

    lseek(fd, 0, SEEK_SET);
    return 0;
}
/*--------------------------------------------------------------------*/
int arc_delete(int fd, char * filename)
{
	struct FILE_DATA  Data;
	struct ARC_HEADER Header;
	off_t read_bytes;
	//lseek(fd, 0, SEEK_SET);
	lseek(fd, sizeof(magic_number), SEEK_SET);

	while((read_bytes = read(fd, &Header, sizeof(struct ARC_HEADER))) > 0)
	{
        if (!strcmp(filename, Header.name))
        {
        	if (Header.deleted)
            {
        		printf("File %s has already been deleted", Header.name);

        		return 1;
        	}
        	else
        	{
            	off_t cur = lseek(fd, 0, SEEK_CUR);
            	lseek(fd, cur - sizeof(struct ARC_HEADER), SEEK_SET);

            	Header.deleted = 1;
            	write(fd, &Header, sizeof(struct ARC_HEADER));

			    lseek(fd, 0, SEEK_SET);

			    return 0;
        	}
        }

        read(fd, &Data, sizeof(struct FILE_DATA));
    }

}

void arc_list(int fd)
{
	struct FILE_DATA Data;
	struct ARC_HEADER Header;
	off_t read_bytes;

	lseek(fd, sizeof(magic_number), SEEK_SET);

	while((read_bytes = read(fd, &Header, sizeof(struct ARC_HEADER))) > 0)
	{
		if (!Header.deleted)
		{
			printf("%s \n", Header.name);
		}

        read(fd, &Data, sizeof(struct FILE_DATA));
	}

	//lseek(fd, 0, SEEK_SET);
}
/*--------------------------------------------------------------------*/
int arc_copy(int fd, char * filename)
{
	struct FILE_DATA Data;
	struct ARC_HEADER Header;
	off_t read_bytes;

	lseek(fd, sizeof(magic_number), SEEK_SET);

	while((read_bytes = read(fd, &Header, sizeof(struct ARC_HEADER))) > 0)
	{
        
        read(fd, &Data, sizeof(struct FILE_DATA));

		if (!strcmp(filename, Header.name))
		{
			if (Header.deleted)
			{
				return 0;
			}
			

			char * new_name = copied_name(filename);

			if (exist(fd, new_name))
			{
				printf("Copy already exists\n");
				return 0;
			}

			strncpy(Header.name, new_name, filenamesize);

			del_files(fd);
			write(fd, &Header, sizeof(struct ARC_HEADER));
			write(fd, &Data,   sizeof(struct FILE_DATA));

			return 0;
		}
	}

	printf("File does not exist\n");
	return 0;

}

char * copied_name(char * filename)
{
	char * copychar;
	char * str = "(copy)";

	if(copychar = malloc(strlen(str) +  strlen(filename)) + 1)
	{
		copychar[0] = '\0';
		strcat(copychar, filename);
		strcat(copychar, str);
	}

	return copychar;
}
/*-----------------------------------------------------------------------*/
int	arc_rename(int fd, char * filename, char * new_filename)
{
	struct FILE_DATA  Data;
	struct ARC_HEADER Header;
	off_t  read_bytes;

	lseek(fd, sizeof(magic_number), SEEK_SET);

	while((read_bytes = read(fd, &Header, sizeof(struct ARC_HEADER))) > 0)
	{
		if (!strcmp(Header.name, filename))
		{
			if (Header.deleted)
			{
				printf("File was deleted\n");
			}

			strncpy(Header.name, new_filename, filenamesize);
			
			off_t cur = lseek(fd, 0, SEEK_CUR);
            lseek(fd, cur - sizeof(struct ARC_HEADER), SEEK_SET);
            write(fd, &Header, sizeof(struct ARC_HEADER));
			
			return 0;			
		}


        read(fd, &Data, sizeof(struct FILE_DATA));
	}	

	printf("File does not exist\n");
	return 0;
}
/*--------------------------------------------------------------------*/
