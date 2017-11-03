#pragma once

#include <fcntl.h>	    // open(), keys 
#include <stdarg.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>	// permission
#include <sys/types.h>	// mode_t 
#include <unistd.h>	    // write(), close() 

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

int    arc_copy    (int, char *); //make a copy 
int    arc_delete  (int, char *); //delete file
void   arc_list    (int);		  //see all files
int    arc_read    (int, char *); //read file data
int	   arc_rename  (int, char *, char *); 
int    arc_write   (int, char *); //write file to archive

char * copied_name (char *); 	  //append (copy)
char * data_read   (char *); 
int    exist       (int, char *); //check file existence
int	   del_files   (int);         // point to deleted file
int	   is_empty    (int);         //check empty archive