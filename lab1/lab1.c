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

            fprintf(fp, "I'm just a child process, a pathetic copy\n My pid = %d \n I'm a parent process! \n  My ppid: %d\n", getpid(), getppid());
                 
            srand(time(NULL));

            char byte;
            for(int i = 0; i < X; i++){
                byte = rand();
                fwrite(&byte, sizeof(char), 1, fp);
            }

            fclose(fp);

    		exit(0);
    	}

    		default: //parent
    		{ 
    			//printf("I'm a parent process!\n");
    			//printf("My pid = %d \n", getppid());
	
    			int status = 0;
	
    			clock_t begin = clock();
    			waitpid(pid, &status, 0);
    			clock_t end   = clock(); 
	
    			printf("Parent process waited child process %f seconds \n", 
    			    (double)(end - begin) / CLOCKS_PER_SEC);
	    	}
	}	
}

