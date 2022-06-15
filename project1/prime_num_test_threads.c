#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>

int max_size = 4500;
pthread_t *t;
int k = 0;
int terminate = 0;
//int turn = 0;

struct thread_stats {
    int free;
    int num_check;
    int notify;
};



int primetest(int number) {
    int i, flag = 0;
    
    if (number == 1 || number == 0) {
      printf("%d is neither a prime nor a composite number.\n", number);
      return 1;
    }
    
    for(i = 2; i <= number/2; ++i) {
        // condition for nonprime number
        if(number%i == 0) {
            //printf("%d is not a prime number.\n");
            flag = 1;
            break;
        }
    }
    
    if (flag == 0) {
          printf("%d is a prime number.\n", number);
          return 1;
        }
        
    else {
          printf("%d is not a prime number.\n", number);
          return 0;

        }

    return 0;

}

void * work(void *args) {
    struct thread_stats *stats = (struct thread_stats*) args;
    int check = 0;

    /*while(1) {
        stats->work_to_do = 0;
        if(terminate) break;
        if(stats->notify == 1) {
            stats->work_to_do = 1;
            stats->notify = 0;
            check = primetest(stats->num_check);
            printf("check = %d\n",check);
            stats->free = 0;

        }
    }*/
    //printf("I AM IN WORK %d\n", stats->num_check);
    while(1){
        //stats->free = 1;
        if(terminate) { return NULL; }
        if(stats->notify == 1 /*&& turn == 0*/){
            stats->notify = 0;
            //turn = 1;
            //stats->free = 0;
            primetest(stats->num_check);
            //turn = 0;
            stats->free = 1;
            
        }
    }
    
}

int main(int argc, char *argv[]) {
    int n, i, j = 0;
    int workers = atoi(argv[1]);
    long int *numbers = NULL;
	
	
    struct thread_stats *thread_array = NULL;
	
    printf("Enter a sequence of %d positive integers: \n",max_size);

    numbers =(long int *)malloc(max_size * sizeof(long int));
	

    for(i = 0; i < max_size; i++){
        scanf(" %ld",&numbers[i]);
    }

    thread_array =(struct thread_stats *)malloc(workers * sizeof(struct thread_stats));
    
    for(j=0; j<workers; j++){
        thread_array[j].free = 1;
        thread_array[j].notify = 0;
    }

    t = (pthread_t *)malloc(workers * sizeof(pthread_t ));
    for(j = 0; j < workers; j++) {
        pthread_create(&t[j], NULL, work, &thread_array[j]); //Default Attributes
    }

    /*while(k < max_size) {

        for(j = 0; j < workers; j++){
            if(thread_array[j].free == 0) {
                thread_array[j].num_check = numbers[k];
                thread_array[j].notify = 1;
                k++;
            }
        }
    }*/
    for(k=0; k<max_size; k++){
        while(1){
            for(j=0; j<workers; j++){
                if(thread_array[j].free == 1){
                    //printf("found worker : %d for number : %d\n", j, numbers[k]);
                    thread_array[j].num_check = numbers[k];
                    thread_array[j].notify = 1;
                    thread_array[j].free = 0;
                    break;
                }
            }
            if(j < workers ){break;}
        }
    }
    
    sleep(1);
    terminate = 1;
	
    free(numbers);
    free(thread_array);
    free(t);
    //fflush(stdout);
    return 0;
}
