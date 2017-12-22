/*8) Создать FIFO. Процесс, который создал FIFO должен записывать
 в FIFO строку с текущим временем каждую секунду. Если процесс запустился
  и FIFO уже создана, он должен начать читать из него данные и
   выводить их на экран.*/

//и снова комменты на русском, и снова извините

#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h> 
#include <signal.h>
#include <pthread.h>

char * fifo_path = "/tmp/die";
int    is_slave  = 1;
int    fifo_fd;
char   buf[11];

void sig_handler(int signo)
{
    if(signo == SIGINT)
    {
        printf("ну ок, received SIGINT\n");
        if(!is_slave)
            unlink(fifo_path); //удаляем имя из ФС
        exit(0);
    }
    if(signo == SIGPIPE){ 
        printf("ну ок, received SIGPIPE\n"); 
        unlink(fifo_path); 
        exit(0);
    }
}

int main(int argc, char** argv)
{
    setbuf(stdout, NULL);
    signal(SIGINT, sig_handler);

    //sigpipe - при отсутствии или обрыве соединения с читающей стороной
    signal(SIGPIPE, sig_handler); 

    struct stat st;
    if(stat(fifo_path, &st) != 0)
    {
        is_slave = 0;
        printf("Беда, нет фифо, ща попробую сделать.\n");
        mkfifo(fifo_path, 0666);
    }

    if(is_slave)
    {
        printf("SLAVE\n");
        fifo_fd = open(fifo_path, O_RDONLY);

        while(1)
        {
            sleep(1);         
            read(fifo_fd, &buf, sizeof(char)*11);
            printf("read: %s \n", buf);
        }
    }
    else
    { 
        printf("MASTER\n");
        fifo_fd = open(fifo_path, O_WRONLY);
        char time_str[100] = "";
        while(1)
        {
            sleep(1);
      		time_t t     = time(NULL);
      		struct tm tm = *localtime(&t);
      		printf("write:");
      		sprintf(time_str, "ID:%p %d:%d:%d parent time\n", pthread_self(), tm.tm_hour, tm.tm_min, tm.tm_sec);
      		printf("%s\n", time_str);
            
            dprintf(fifo_fd, "%u\n", t); 
        }
    }
}   