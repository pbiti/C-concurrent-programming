/* Tasiou Evangelia 2636
 * Biti Polyxeni 2582 */

#define _GNU_SOURCE
#define OPEN_THREADS

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int SIZE=64;

char *ring_buf; 
int read_pt;
int write_pt;
int closed = 0; //flag pipe_closed
int write_stop = 0; 
int read_stop = 1; //arxika einai 1 giati prepei prwta na grafei kati 
int write_done = 0; //eggrafh accomplished 
off_t file_size;



void pipe_init (int size) {
    int i;
    
    ring_buf = (char *)malloc(size*sizeof(char));
    if(ring_buf == NULL){
        printf("Problem allocating memory for the pipe\n");
        exit(-1);
    }
}

void pipe_write (char c) {
	
	ring_buf[write_pt] = c;
}

void pipe_close() {
    free(ring_buf);
    closed = 1; //gia elegxo sth main 
}


int pipe_read (char *c) {
 
    if (closed==1) {
        return(0);
    }
    else{
        *c = ring_buf[read_pt];
        return(1);
    }
        
}

//kalei tin pipe_write oses fores xreiastei gia na grapsei ton kathe char

void *thread_write (void *arg) {
    int i;
    char *data; 
    data = (char *) arg;
    
    for (i=0; i<file_size; i++){
        if (write_stop == 0) { //checks if it's permitted to write

            pipe_write(data[i]);
            write_pt = ((write_pt+1)%SIZE); //changes the write pointer
            
			//checks if it can continue writing, elegxei wste na mhn kanei overwrite data pou den exoun diavastei
			if(write_pt == read_pt){
                write_stop = 1;
                read_stop = 0; //h read etoimh na diavasei 
                pthread_yield();
            }
            else {
                
                write_stop = 0; 
                read_stop = 0;
            }
            
        }
        else{
            i--; //an den mporw na kanw write, meiwnw counter wste na mhn prosperasw kapoio char
        }
    }
    
    write_done = 1; //informs that it's done
    
    return(NULL);
}

void *thread_read (void *arg) {
    
    int i;
    char temp_byte, *read_byte;
	ssize_t wr1;
    
    read_byte=&temp_byte;
    
    for (i=0; i<file_size; i++) {
    
        if(read_stop == 0) { //checks if it's permitted to read 
            pipe_read(read_byte); 
            
            printf("%c", temp_byte);
			
            read_pt = ((read_pt+1)%SIZE);
            
            //shmainei oti h thesi tou pinaka einai adeia kai den exei grafei kati 
			if(write_pt == read_pt){
                read_stop = 1;
                write_stop = 0;
                pthread_yield();
            }
        }
        else { 
            if(write_done==1){
                break;
            }
            i--; //meiwnei epanalhpsh wste na mh xathei to read kapoiou char
            pthread_yield(); 
        }
    }
    
    if(write_done==1){
        pipe_close();
    }   
    return(NULL);
}


int main (int argc, char *argv[]) {
    int thread_rd, thread_wr, i;
    char *data;
    pthread_t t1, t2;
    int fd_file, cl1;
	off_t ls2;
	ssize_t rd1;

    pipe_init(SIZE); //pipe 64 bytes 
    
    read_pt = 0; //arxikopoiisi ton pointers stin proti thesi tou pipe
    write_pt = 0;
    
	
	fd_file = open(argv[1], O_RDONLY);
	if(fd_file<0){
		perror("opening the file");
		exit(1);
	}
	
	file_size = lseek(fd_file, 0, SEEK_END); //finding the size of the file to be imported
	if(file_size<0){
		perror("lseek to find size of the file");
		exit(1);
	}
	
	
	ls2 = lseek(fd_file, 0, SEEK_SET); //lseek to start 
	if(ls2<0){
		perror("lseek to start of the file");
		exit(1);
	}
	
	data = (char *)malloc(file_size*(sizeof(char)));
	if(data == NULL){
		perror("allocating mem for data array");
		exit(1);
	}
	
	for(i=0; i<file_size; i++){
        
		rd1 = read(fd_file, &data[i], sizeof(char));
		if(rd1<0){
			perror("read char");
			exit(1);
		}
    }
    
    
    thread_rd = pthread_create (&t1, NULL, thread_read, NULL );
    if (thread_rd != 0){
        printf("Thread for reading had problem initiating\n");
    }
    
    thread_wr = pthread_create (&t2, NULL, thread_write, (void *) data);
    if (thread_wr != 0){
        printf("Thread for writing had problem initiating\n");
    }
    
    
    while (1) {
        if(closed==1){
            break;
        }
    };
	cl1 = close(fd_file);
	if(cl1<0){
		perror("closing file");
		exit(1);
	}
	
    free(data);
	
    return(0);
}
