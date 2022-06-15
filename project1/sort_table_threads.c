#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#define MAX 11

struct Threadings {
    int left; 
    int right;
    int sorted;
};

int intArray[MAX];

void swap(int num1, int num2) {
    int temp = intArray[num1];
    intArray[num1] = intArray[num2];
    intArray[num2] = temp;
}

int partition(int left, int right, int pivot) {
    int leftPointer = left-1; 
    int rightPointer = right;
    int i;

    while(true) {
    while(intArray[++leftPointer] < pivot) { //left psaxnei gia stoixeia megalytera tou pivot 
         //do nothing
    }
		
    while(rightPointer > 0 && intArray[--rightPointer] > pivot) { //o right psaxnei gia stoixeia mikrotera tou pivot 
         //do nothing
    }

    if(leftPointer >= rightPointer) {
        break;
    } 
    else {
        //printf(" item swapped :%d,%d\n", intArray[leftPointer],intArray[rightPointer]); //kanw swap stoixeia 
        swap(leftPointer,rightPointer);  
    }
    }
   
    //printf(" pivot swapped :%d,%d\n", intArray[leftPointer],intArray[right]);
    swap(leftPointer,right);
    /* printf("Updated Array: ");
    for(i=0; i<MAX; i++){
        printf("%d ", intArray[i]);
    }
    
    printf("\n"); */
    return leftPointer;
}

void *quick_thread(void *arg) {
    
    struct Threadings ts1; //thugatriko thread 1
    struct Threadings ts2; //thugatriko thread 2 
    struct Threadings *self_ts; //info tou mother thread 
    
    pthread_t thread_id;
    
    void *t1_pt;
    void *t2_pt;
    
    self_ts = (struct Threadings *)arg;
    thread_id=pthread_self();
    
    //printf("\n-> Welcome to thread %lu. right= %d, left= %d, sorted= %d \n", thread_id, self_ts->right, self_ts->left, self_ts->sorted);
    t1_pt = &ts1;
    t2_pt = &ts2;
    

    int thread1, thread2;
    pthread_t t1, t2;
    
    ts1.sorted = 0;
    ts2.sorted = 0;
    
    
   if(self_ts->right-self_ts->left <= 0) { //an o pinakas exei 2 stoixeia 
      self_ts->sorted = 1;   
   } 
   else {
      int pivot = intArray[self_ts->right];
      int partitionPoint = partition(self_ts->left, self_ts->right, pivot);
       
      ts1.right = partitionPoint - 1;
      ts1.left = 0;
       
      thread1 = pthread_create(&t1, NULL, quick_thread, (void *) t1_pt);
      if(thread1 !=0) {
          printf("Problem while creating thread1!! \n");
       }
       ts2.left = partitionPoint + 1; 
       ts2.right = self_ts->right;
       
      thread2 = pthread_create(&t2, NULL, quick_thread, (void *) t2_pt);
       if(thread2 !=0) {
        printf("Problem while creating thread2!! \n");
    }
    
   }
    while (1) {
        if((ts1.sorted == 1) && (ts2.sorted == 1)) { //both subsidiary threads sorted 
            self_ts->sorted = 1;
            break;
        }
    };
    return(NULL);
}




int main(int argc, char *argv[]) {
    
    int i, thread;
    pthread_t t1; 
    
    struct Threadings t;
    
    void *struct_pt;
    
    struct_pt = &t;
    

    t.sorted = 0;
    
    t.left = 0; 
    t.right = MAX-1;
    
    printf("Give %d different integers\n", MAX);
    for(i=0; i<MAX; i++){
        scanf("%d", &intArray[i] );
    }
    
    //printf("\n");
   
    thread = pthread_create(&t1, NULL, quick_thread,(void *) struct_pt);
    
    if(thread !=0) {
        printf("Problem while creating thread!! \n");
    }
    
    while (1) {
        if(t.sorted == 1) {
            break;
        }
    };
	
    printf("Sorted array: ");
    for(i=0; i<MAX; i++){ 
        printf("%d ", intArray[i]);
    }
    
    printf("\n");
    
    return(0);

}
