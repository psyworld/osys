/*6) Аналогично лаб4, но для синхронизации использовать
блокировку чтения-записи*/

#include <stdio.h> 
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h> 
#include <signal.h>
#include <pthread.h>

#define SHM_SIZE 128

#define NUM_THREADS 5

int    shm_id;
void * shmem;

int    is_slave = 1;

pthread_rwlock_t lock;
pthread_t        tid[NUM_THREADS];

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
    int id = *((int*)args);
    char time_str[100] = "";
    while(1)
    {
        sleep(rand()%5);
        pthread_rwlock_wrlock(&lock);

        time_t t = time(NULL);
    	struct tm tm = *localtime(&t);
    	sprintf(time_str, "ID:%p %d:%d:%d parent time\n", pthread_self(), tm.tm_hour, tm.tm_min, tm.tm_sec);
    	printf("%s\n", time_str);
    	
    	sprintf((char*) (shmem), "%s\n", time_str);
    	pthread_rwlock_unlock(&lock);
    }
    return NULL;
}

void* read_thread(void* args)
{
    int id = *((int*)args);

    while(1)
    {
        pthread_rwlock_rdlock(&lock);
        printf("%d - %s\n", id, (char*) shmem);
        sleep(1);
        pthread_rwlock_unlock(&lock);
    }
    return NULL;
}

int main(int argc, char** argv)
{
    setbuf(stdout, NULL);
    signal(SIGINT, sig_handler);

    pthread_rwlock_init(&lock, NULL);

    key_t shm_key = ftok("/dev/random", 666);
    printf("ftok key %u\n", shm_key);

    shm_id = shmget(shm_key, SHM_SIZE, 0);

    if(shm_id == -1)
    {
        is_slave = 0;
        shm_id = shmget(shm_key, SHM_SIZE, IPC_CREAT | 0666);

        if(shm_id == -1){
            printf("Расходимся, shmget error\n");
        }
    }

    shmem = shmat(shm_id, NULL, 0); //подстыковываем кусок shm_id к адресному пр-ву выз. процесса
    if(shmem == (void*)-1)
    { 
        printf("Беда, не подстыковать\n");
        exit(-1);
    }


    if(is_slave){
        printf("SLAVE\n");

        int i = 0;
        for(; i < NUM_THREADS; i++){
            printf("Создаем поток! %d\n", i);
            pthread_create(&(tid[i]), NULL, &read_thread, &i);
        }
    }
    else{
        printf("MASTER\n");
        
        int i = 0;
        for(; i < NUM_THREADS; i++){
            printf("Создаем поток! %d\n", i);
            pthread_create(&(tid[i]), NULL, &write_thread, &i);
        }
    }

    int i = 0;
    for(; i < NUM_THREADS; i++){
        pthread_join(tid[i], NULL);
    }
    
    shmdt(shmem);
  	shmctl(shm_id, IPC_RMID, NULL);

  	return 0;
}