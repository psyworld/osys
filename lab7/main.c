/*7) Создать область разделяемой памяти.В рамках одного процесса
 запустить один поток, который должен писать в shmem блоки данных
  по 16 КБ, и несколько поток читающих из shmem. Синхронизировать обмен
   данными с помощью условных переменных.*/

#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h> 
#include <signal.h>
#include <pthread.h>

#define NUM_THREADS_RD 5
#define BLOCK_SIZE     1024 * 16
#define SHM_SIZE       16 * BLOCK_SIZE

int    shm_id;
void * shmem;
int    is_slave = 1;

pthread_mutex_t lock;
pthread_cond_t  cond;
pthread_t       tid_rd[NUM_THREADS_RD];
pthread_t       tid_wr[1];

int last_block_written = -1;

void sig_handler(int signo)
{ //чо будем будем делать, если ctrl+c приедет
    if(signo == SIGINT)
    {
        printf("\n ну ок, received SIGINT\n");
        shmdt(shmem); //отстыковываем сегмент разделяемой памяти

        if(!is_slave)
            shmctl(shm_id, IPC_RMID, NULL); //удалям сегмент
        
        exit(0);
    }
}

void* write_thread(void* args)
{
    unsigned block_n = 0;
    while(1)
    {
        pthread_mutex_lock(&lock);
        printf("ID: %p write \n", pthread_self());
        size_t i = 0;

        for(; i < 1024 * 16; i++) //запись рандомной штуки
            ((char*)shmem)[(block_n * BLOCK_SIZE) + i] = '0' + (char)(rand()%9);


        last_block_written = block_n;

        if(++block_n >= 16)
            block_n = 0;

        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&lock);
        sleep(rand()%3);
    }

    return NULL;
}

void* read_thread(void* args)
{
    int id = *((int*)args);
    while(1)
    {
        pthread_mutex_lock(&lock);

        while(last_block_written == -1)
            pthread_cond_wait(&cond, &lock);

        printf("ID: %p read:",  pthread_self());

        int i = 0;
        for(; i < 16; i++)
            printf("%c", *((char*)shmem + last_block_written * BLOCK_SIZE + i));
        printf("\n");
        
        sleep(rand()%3);
        last_block_written = -1;
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}

int main(int argc, char** argv)
{
    setbuf(stdout, NULL);
    signal(SIGINT, sig_handler);

    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    key_t shm_key = ftok("/dev/random", 666);
    printf("ftok key %u\n", shm_key);
    
    shm_id   = shmget(shm_key, SHM_SIZE, IPC_CREAT | 0666); //создаем сегмент, даем права
    if(shm_id == -1)
        printf("Расходимся, shmget error\n");

    shmem = shmat(shm_id, NULL, 0); //подстыковываем кусок shm_id к адресному пр-ву выз. процесса
    
    if(shmem == (void*)-1)
    { 
        printf("Беда, не подстыковать\n");
        exit(-1);
    }


    int i = 0;
    for(; i < NUM_THREADS_RD; i++){
        pthread_create(&(tid_rd[i]), NULL, &read_thread, &i);
    }

    pthread_create(&(tid_wr[0]), NULL, &write_thread, NULL);
    pthread_join(tid_wr[0], NULL);
}