#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>	    // write(), close() 
#include <fcntl.h>	    // open(), keys 
#include <sys/stat.h>	// permission
#include <sys/types.h>	// mode_t 
#include <stdarg.h> 

void   myprinf(const char *, char *, ...);
char * convert(unsigned int, int);
size_t my_strlen(char *);

int main (int argc, char ** argv)
{

	myprinf(0, "%s %d %o %c %x %c", 
		":( My ugly printf:" , 666, 666, '6', 666, '\n');

	return 0;
}

void myprinf(const char * id, char * format, ...)
{
	char *	 	 iterator;
	char * 	 	 s;
	unsigned int 	 i;
	ssize_t  	 read_bytes;
	ssize_t  	 written_bytes;
	int 	 	 fd;
	va_list  	 arg;
	va_start(arg, format);

	if (id == 0)
	{
		fd = 1;
	}
	else 
	{
		fd = open(id, O_RDWR | O_CREAT | O_APPEND, 
			         S_IRWXU | S_IRWXG | S_IRWXO);
	}
		
	for (iterator = format; *iterator != '\0'; iterator++)
	{
		while(*iterator != '%' && *iterator != '\0')
		{
			i = *iterator;
			void * data[0];
			int  * p;
			p 		= malloc(sizeof *p);
			data[0] = p;
			*p 		= i;	

			written_bytes = write (fd, data[0], my_strlen(iterator));
			iterator++;

			free(p);
		}

		iterator++;

		switch(*iterator)
		{
			case 'c': 

				i = va_arg(arg,int);
				void * data[10];
				int  *p;
				p 		= malloc(sizeof *p);
				data[0] = p;
				*p 		= i;

				written_bytes = write (fd, data[0], sizeof(char));

				free(p);

				break;
			case 'd':

				i = va_arg(arg,int);
				written_bytes = write (fd, convert(i,10), my_strlen(convert(i,10)));

				break;
			case 'o':

				i = va_arg(arg,unsigned int);
				written_bytes = write (fd, convert(i,8), my_strlen(convert(i,8)));

				break;
			case 's':

				s = va_arg(arg,char *);
				written_bytes = write (fd, s, my_strlen(s));

				break;
			case 'x':
				
				i = va_arg(arg, unsigned int);
				written_bytes = write (fd, convert(i,16), my_strlen(convert(i,16)));
				break;
		}
	}

	if (id == 0) 
	{
		close(fd);
	}

	va_end(arg);
}

size_t my_strlen(char * s) 
{
    const char* cur = s;
    for (; *cur; ++cur);
    return cur - s;
}

char * convert(unsigned int num, int base) 
{ 
	static char Representation[] = "0123456789ABCDEF";
	static char buffer[50]; 
	char * 		ptr; 
	
	ptr  = &buffer[49]; 
	*ptr = '\0'; 
	
	do 
	{ 
		*--ptr = Representation[num%base]; 
		num   /= base; 

	} while (num != 0); 
	
	return(ptr); 
}
