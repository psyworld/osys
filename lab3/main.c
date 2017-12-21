/*3) Создать область разделяемой памяти. Процесс, который
 создал область должен записывать в эту область строку с
  текущим временем каждую секунду. Если процесс запустился
   и область разделяемой памяти уже создана, он должен
    начать читать из нее данные и выводить их на экран.*/

//мне так лень писать комменты на английском языке, 
//извините, если что

#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h> 
#include <signal.h> 

const int SHM_SIZE = 128;
int       is_slave =   1;

int    shm_id;
void * shmem;

void sig_handler(int signo){ //чо будем будем делать, если ctrl+c приедет
    if(signo == SIGINT)
    {
        printf("\n ну ок, received SIGINT\n");
        shmdt(shmem); //отстыковываем сегмент разделяемой памяти

        if(!is_slave)
            shmctl(shm_id, IPC_RMID, NULL); //удалям сегмент
        
        exit(0);
    }
}

int main(int argc, char** argv)
{
    setbuf(stdout, NULL); //ставим буфер для записи в поток данных
    signal(SIGINT, sig_handler); //стережем Ctrl+C

    key_t shm_key = ftok("/dev/random", 1); //создаем ключик
    printf("ftok key %u\n", shm_key);

    shm_id = shmget(shm_key, SHM_SIZE, 0); //даем куску памяти id ~ shm_key
    
    if(shm_id == -1)
    {
        is_slave = 0;
        shm_id   = shmget(shm_key, SHM_SIZE, IPC_CREAT | 0666); //создаем сегмент, даем права
        
        if(shm_id == -1)
            printf("Расходимся, shmget error\n");
    }

    shmem = shmat(shm_id, NULL, 0); //подстыковываем кусок shm_id к адресному пр-ву выз. процесса
    if(shmem == (void*)-1)
    { 
        printf("Беда, не подстыковать\n");
        exit(-1);
    }

    if(is_slave)
    {
        
        while(1) //читаем из памяти
        { 
            sleep(1);         
            printf("<--| %s", (char*) shmem);
        }
    }
    else
    {
    	char time_str[100] = "";
        while(1) //записываем время
        { 
		    sleep(1);
		    time_t t     = time(NULL);
		    struct tm tm = *localtime(&t);
		    sprintf(time_str, "%d:%d:%d parent time\n", tm.tm_hour, tm.tm_min, tm.tm_sec); //адкватная запись времечка
		    printf("%s\n", time_str);
		    sprintf((char*) (shmem), "%s ", time_str);
        }
    }
    
    //ну на всякий
    shmdt(shmem);
    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}