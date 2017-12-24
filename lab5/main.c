/*5) Создать область разделяемой памяти. Процесс, который создал
 область должен записывать в эту область строку с текущим временем
  каждую секунду. Если процесс запустился и область разделяемой 
  памяти уже создана, он должен начать читать из нее данные и выводить
   их на экран.Доступ к разделяемой памяти синхронизировать 
    с помощью семафора.*/

//мне так лень писать комменты на английском языке, 
//извините, если что

#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <unistd.h> 
#include <signal.h> // sigaction(), sigsuspend(), sig*()

#define SHM_SIZE 128

int    shm_id;
void * shmem;
int    is_slave = 1;

int    semid;
struct sembuf sb;

void sig_handler(int signo) //принимаем Ctrl+C 
{
    if(signo == SIGINT)
    {
        printf("\n ну ок, received SIGINT\n");
        shmdt(shmem);

        if(!is_slave)
        {
            shmctl(shm_id, IPC_RMID, NULL);
            semctl(semid, 0, IPC_RMID);
        }
        
        exit(0);
    }
}

int main(int argc, char** argv)
{
    setbuf(stdout, NULL);
    signal(SIGINT, sig_handler);

    key_t shm_key = ftok("/dev/random", 666); //создаем ключик
    printf("ftok key %u\n", shm_key);

    shm_id = shmget(shm_key, SHM_SIZE, 0); //даем куску памяти id

    if(shm_id == -1)
    {
        is_slave = 0;
        shm_id = shmget(shm_key, SHM_SIZE, IPC_CREAT | 0666);

        if(shm_id == -1){
            printf("Расходимся, shmget error\n");
        }
    }

    shmem = shmat(shm_id, NULL, 0);

    if(shmem == (void*)-1)
    {
        printf("Беда, не подстыковать\n");
        exit(-1);
    }

    sb.sem_num = 0;
    sb.sem_flg = 0;

    if(is_slave)
    {
        printf("SLAVE\n");

        semid = semget(shm_key, 1, 0660);
        
        while(1)
        {  

            sb.sem_op = -1; // lock
            //semop - ф-ия для манипулирования значениями семафоров

            if (semop(semid, &sb, 1) == -1)
            {
                printf("Беда, semop error\n");
            	return 0;
            };
            printf("read: %s\n", (char*) shmem);

        }
        return 0;
    }
    else
    {
        printf("MASTER\n");
        char time_str[100] = "";

        semid = semget(shm_key, 1, IPC_CREAT | IPC_EXCL | 0666); //создаем семафор
        
        semctl(semid, 0, SETVAL, 1);

        while(1)
        {
            sleep(1);

            time_t t     = time(NULL);
            struct tm tm = *localtime(&t);

            printf("write:");
            sprintf(time_str, "%d:%d:%d parent time\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
            printf("%s\n", time_str);
      
            sprintf((char*) (shmem), "%s\n", time_str);
            
            sb.sem_op = 1; // unlock
            semop(semid, &sb, 1);
        }
    }
    return 0;
}