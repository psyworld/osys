/* 4) Изменить лаб3 следующим образом: пишущий в разделяемую память
 процесс должен запустить несколько (2 < n < 10) отдельных потоков,
  которые получают значение времени и записывают в разделяемую память
   в виде строки "THREAD_ID - время". Дочерний процесс должен
    запускать несколько поток для чтения. Доступ к разделяемой 
    памяти должен быть синхронизирован в рамках каждого процесса
     мьютекс; */

#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h> 
#include <signal.h>
#include <pthread.h>

#define SHM_SIZE    128
#define NUM_THREADS   5

int    shm_id;
void * shmem;
int is_slave = 1;

pthread_mutex_t lock;
pthread_t       tid[NUM_THREADS];

void sig_handler(int signo) //ну тут все как в 3 лабочке
{
    if(signo == SIGINT)
    {
        printf("\n ну ок, received SIGINT\n");
        shmdt(shmem);

        if(!is_slave)
            shmctl(shm_id, IPC_RMID, NULL);        
        
        exit(0);
    }
}

void * write_thread(void * args) //пишем айдишник потока и времечко
{
    int id = *((int*)args); //странные вещи
    char time_str[100] = "";

    while(1)
    {
        sleep(rand()%5 + 1);
        pthread_mutex_lock(&lock); //блокируем мьютекс

	    time_t t     = time(NULL);
	    struct tm tm = *localtime(&t);
	    sprintf(time_str, "ID:%p %d:%d:%d parent", pthread_self(), tm.tm_hour, tm.tm_min, tm.tm_sec);
	    printf("%s\n", time_str);

	    sprintf((char*) (shmem), "%s ", time_str);
	    sleep(rand()%3);
	    pthread_mutex_unlock(&lock);
    }

    return NULL;
}

void * read_thread(void * args)
{
    int id = *((int*)args); //странные вещи [2]

    while(1)
    {
        sleep(rand()%5 + 1);
        pthread_mutex_lock(&lock); //лочим
        printf("[%d] - %s", id, (char*) shmem);
        sleep(rand()%3);
        pthread_mutex_unlock(&lock); //анлочим
    }

    return NULL;
}

int main(int argc, char** argv)
{
    setbuf(stdout, NULL);
    signal(SIGINT, sig_handler);

    pthread_mutex_init(&lock, NULL); //инициализируем мьютекс

    key_t shm_key = ftok("/dev/random", 1);
    printf("ftok key %u\n", shm_key);

    shm_id = shmget(shm_key, SHM_SIZE, 0);

    if(shm_id == -1)
    {
        is_slave = 0;
        shm_id = shmget(shm_key, SHM_SIZE, IPC_CREAT | 0666);
    
        if(shm_id == -1)
            printf("Расходимся, shmget error\n");
    }

    shmem = shmat(shm_id, NULL, 0);

    if(shmem == (void*)-1)
    {
        printf("Беда, не подстыковать\n");        
        exit(-1);
    }

    if(is_slave)
    {
        int i = 0;
        for(; i < NUM_THREADS; i++)
        {
            printf("Создаем поток! %d\n", i);
            pthread_create(&(tid[i]), NULL, &read_thread, &i); //поток на чтение
        }
    }
    else
    {

        int i = 0;
        for(; i < NUM_THREADS; i++)
        {
            printf("Создаем поток! %d\n", i);
            pthread_create(&(tid[i]), NULL, &write_thread, &i); //поток на запись
        }
    }


	pthread_join(tid[0], NULL);
	pthread_mutex_destroy(&lock);
	shmdt(shmem);
	shmctl(shm_id, IPC_RMID, NULL);

  	return 0;


}