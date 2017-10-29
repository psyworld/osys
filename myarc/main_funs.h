#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>	    // write(), close() 
#include <fcntl.h>	    // open(), keys 
#include <sys/stat.h>	// permission
#include <sys/types.h>	// mode_t 
#include <stdarg.h> 
#include <string.h>

extern const int filesize;
extern const int filenamesize;
extern const int magic_number;

struct ARC_HEADER
{
	char  name[128];
	int   deleted;
};

struct FILE_DATA
{
	char data[1024];

};

int    arc_write (int, char *);
char * data_read (char *);
int	   is_empty  (int);	
int    arc_read  (int, char *);
int    exist     (int, char *);
int    arc_delete(int, char *);
void   arc_list  (int);