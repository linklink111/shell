#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include "myipc.h"
// 改自consumer.c
int main(int argc, char *argv[])
{
    int i,item,shmid;
	semaphore mutex,apple, empty;
    union semun sem_union;
	void *shared_memory = (void *)0;
	struct shared_use_st *shared_stuff;
	// 以下是新定义的变量
	
	// 尝试获得独立运行的 semaphore
	if ( (mutex=semget((key_t)KEY_MUTEX,1,IPC_EXCL)) == -1 ) {
		fprintf(stderr,"Failed to create semaphore!"); 
		exit(EXIT_FAILURE);
	}
	// 尝试获得修改共享内存的 semaphore
	if ( (shmid = shmget((key_t)KEY_SHM,sizeof(struct shared_use_st),IPC_EXCL)) == -1 ) {
		fprintf(stderr,"Failed to create shared memory!"); 
		exit(EXIT_FAILURE);
	}
	// 尝试获得共享内存
	if ( (shared_memory = shmat(shmid,(void *)0,0) ) == (void *)-1) {
		fprintf(stderr,"shmat failed\n");
		exit(EXIT_FAILURE);
	}
	// 要共享的东西
	shared_stuff = (struct shared_use_st *)shared_memory;
	// 以下是新加入的 semget 及其错误处理
	// 爸爸只需要拿到 apple 的semaphore
	if ( (apple=semget((key_t)KEY_APPLE,1,IPC_EXCL)) == -1 ) {
		fprintf(stderr,"Failed to create semaphore!"); 
		exit(EXIT_FAILURE);
	}
	if ( (empty=semget((key_t)KEY_EMPTY,1,IPC_EXCL)) == -1 ) {
		fprintf(stderr,"Failed to create semaphore!"); 
		exit(EXIT_FAILURE);
	}

	for(i=0;i<30;i++){
	// use loop
		sleep(1);
		printf("The father goes to fetch apple);
		sem_p(empty);
		sem_p(mutex);
		shared_stuff->apple_num += 1;
		printf("The father is inserting apple, now the number of apple is number %d\n",shared_stuff->apple_num);
		sem_v(mutex);
		sem_v(apple);
		sleep(2);
	}

    if (shmdt(shared_memory) == -1) {
    	fprintf(stderr, "shmdt failed\n"); 
		exit(EXIT_FAILURE);
	}
	printf("Finish!\n");
	getchar();
  	exit(EXIT_SUCCESS);
}
