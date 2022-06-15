#include <sys/types.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h> 
#include <sys/sem.h>
#include <stdio.h>


int mysem_create(key_t key, int init[], int nofsems){
	

	int sem_id, sem_ctl;
	
	sem_id = semget(key, nofsems, IPC_CREAT | IPC_EXCL | 0666);
	if(sem_id < 0){
		perror("semget");
		exit(1);
	}
	if(nofsems > 1){
		sem_ctl = semctl(sem_id, 0, SETALL, init);
		if(sem_ctl < 0){
			perror("semctl create 1");
			exit(1);
		}
	}else{
		sem_ctl = semctl(sem_id, 0, SETVAL, init);
		if(sem_ctl < 0){
			perror("semctl create 2");
			exit(1);
		}
	}
	
	return(sem_id);
}

int mysem_down(int sem_id, int semnum){
	
	int sem_down;
	struct sembuf op;
	
	op.sem_num = semnum;
	op.sem_op = -1;
	op.sem_flg = 0;
	
	sem_down = semop(sem_id, &op, 1);
	if(sem_down == -1){
		perror("semop down");
		exit(1);
	}
	return(0);
}

int mysem_up(int sem_id, int semnum){
	
	int sem_up, sem_val;
	struct sembuf op;
	
	op.sem_num = semnum;
	op.sem_op = 1;
	op.sem_flg = 0;
	
	sem_val = semctl(sem_id, semnum, GETVAL);
	if(sem_val < 0 ){
		perror("semctl value");
		exit(1);
	}
	if(sem_val == 1){
		printf("Semaphore is already up!\n");
		return(-1);
	}else{
		sem_up = semop(sem_id, &op, 1);
		if(sem_up < 0){
			perror("semop up");
			exit(1);
		}
	}
	
	return(0);
}

int mysem_destroy(int sem_id){
	
	int sem_dlt;
	
	sem_dlt = semctl(sem_id, 0, IPC_RMID);
	if(sem_dlt < 0){
		perror("semctl delete");
		exit(1);
	}
	return(0);
}


