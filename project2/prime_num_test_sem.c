/* Biti Polyxeni    2582
 * Tasiou Evangelia 2636
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "my_sem.h" 
#include <sys/types.h>
#include <sys/ipc.h>

#define ALL_DONE 2
#define GOT_WORK 1
#define BUSY 0

pthread_t *t;
int k = 0, check=0;
key_t key_semid;

struct thread_stats {
    int semid; //semaphores
    int *num_check;
    int terminate; 
}info;


int primetest(int number, int pid) {
    int i, flag = 0;
    
    if(number == 1 || number == 0){
      printf(ANSI_COLOR_CYAN"Thread %u: %d is neither a prime nor a composite number."ANSI_COLOR_RESET"\n", pid, number);
      return 1;
    }
    
    for(i = 2; i <= number/2; ++i){
        // condition for nonprime number
        if(number%i == 0){
            flag = 1;
            break;
        }
    }
    
    if(flag == 0){
		printf(ANSI_COLOR_GREEN"Thread %u: %d is a prime number."ANSI_COLOR_RESET"\n", pid, number);
		return 1;
	}
    else{
		printf(ANSI_COLOR_RED"Thread %u: %d is not a prime number."ANSI_COLOR_RESET"\n", pid, number);
		return 0;

	}

    return 0;
}

void* work(void *args) {
    struct thread_stats *stats = (struct thread_stats*) args;
 
	printf("Worker %u waiting..\n", (int)pthread_self());
	mysem_down(stats->semid, BUSY); //worker is now busy
    while(1){
		
        if(stats->terminate){ 
			check = mysem_up(stats->semid, ALL_DONE); //no work to do
			
			printf("Worker %u exiting..\n", (int)pthread_self());
			return NULL;
		}
		check = mysem_up(stats->semid, GOT_WORK); //worker got a number

		primetest(*(stats->num_check), (int)pthread_self());
	
		mysem_down(stats->semid, BUSY);
		
	}
	return NULL;
}

int main(int argc, char *argv[]) {
    int i, j = 0, count;
    int workers = atoi(argv[1]);
	int max_size = atoi(argv[2]);
    int number, thread_val;
	int seminit[] = {0, 0, 0};
	
	struct thread_stats *thread_array = NULL;
	if(argc != 3){
		printf("ERROR:wrong number of arguments!\n");
		return (-1);
	}

	key_semid = ftok(argv[0], 'a');
	if(ftok < 0){
		fprintf(stderr, "Key_semid ftok");
		exit(1);
	}
	info.semid = mysem_create(key_semid, seminit, 3);
	
    printf("Enter a sequence of %d positive integers: \n", max_size);

    thread_array =(struct thread_stats *)malloc(workers * sizeof(struct thread_stats));
	if(thread_array == NULL){
		fprintf(stderr, "Malloc - allocating thread_array");
		exit(1);
	}
    
    t = (pthread_t *)malloc(workers * sizeof(pthread_t ));
	if(t == NULL){
		fprintf(stderr, "Malloc - allocating t");
		exit(1);
	}
    for(j = 0; j < workers; j++){
		thread_array[j].semid = info.semid;
		thread_array[j].num_check = &number;
        thread_val = pthread_create(&t[j], NULL, work, &thread_array[j]); //Default Attributes
		if(thread_val){
			perror("Creating threads");
			exit(1);
		}
    }
  
	for(count=0; count < max_size; count++){
		
		scanf("%d\n", &number);
		check = mysem_up(info.semid, BUSY); //main assigned job to a worker BUSY
		if(check == -1){
			exit(1);
		}
		mysem_down(info.semid, GOT_WORK); //value has been assigned 

	}
	 
    for(i=0; i<workers; i++){
		thread_array[i].terminate = 1;
	}
	for(i=0; i<workers; i++){
		check = mysem_up(info.semid, BUSY); //BUSY
		if(check == -1){
			exit(1);
		}
		mysem_down(info.semid, ALL_DONE); //SLEEP
	}
   
	sleep(1);
    free(thread_array);
    free(t);
	mysem_destroy(info.semid);
	
    fflush(stdout);
    return 0;
}
