#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

pid_t fork(void);

int main(int argc, char ** argv) 
{

	pid_t pid;

	switch(pid = fork())
	{
		case -1:

			perror("fork");
			exit(1); 

		case 0: //child
		{

			//creating 10Mb file
			int X = 1024 * 1024 * 10;
			FILE* fp = fopen("myfile", "w");
    		fseek(fp, X , SEEK_SET);
    		fputc('\0', fp);
    		fclose(fp);

    		printf("I'm just a child process, a pathetic copy\n");
    		printf("My pid = %d \n", getpid());

    		exit(0);
    	}

    	default: //parent
    	{ 
    		printf("I'm a parent process!\n");
    		printf("My pid = %d \n", getppid());

    		int status = 0;

    		clock_t begin = clock();
    		waitpid(pid, &status, 0);
    		clock_t end   = clock(); 

    		printf("Parent process waited child process %f seconds \n", 
    			    (double)(end - begin) / CLOCKS_PER_SEC);

    	}
	}	
}

