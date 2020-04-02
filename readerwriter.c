#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct _rwlock{
    sem_t lock;      //binary semaphore (basic lock)
    sem_t writelock; //used to allow one writer or many readers
    int readers;     //count of readers reading in critical section
} rwlock_t;

void rwlock_init(rwlock_t *rw){
    rw->readers = 0;
    sem_init(&rw->lock, 0, 1);
    sem_init(&rw->writelock, 0, 1);
}

void rwlock_acquire_readlock(rwlock_t *rw){
    sem_wait(&rw->lock);
    rw->readers++;
    if(rw->readers == 1)
        sem_wait(&rw->writelock); //first reader acquires writelock
    sem_post(&rw->lock);
}

void rwlock_release_readlock(rwlock_t *rw){
    sem_wait(&rw->lock);
    rw->readers--;
    if(rw->readers == 0)
        sem_post(&rw->writelock); //last reader releases writelock
    sem_post(&rw->lock);
}

void rwlock_acquire_writelock(rwlock_t *rw){
    sem_wait(&rw->writelock);
}

void rwlock_release_writelock(rwlock_t *rw){
    sem_post(&rw->writelock);
}

//This function wastes time to simulate reading/writing
//The void* return and argument are necessary to conform to pthread_create()
void wasteTime(){
    int x = 0, i, j, T;
    T = rand()%10000;
    for(i = 0; i < T; i++){
        for(j = 0; j < T; j++){
            x = i * j;
        }
    }
}

//global variables
rwlock_t lock;
int value = 0; //variable to track the value being read and written

void *reader(void *arg) {
    int i;
	rwlock_acquire_readlock(&lock);
    wasteTime();
	printf("Finished reading value: %d\n", value);
	rwlock_release_readlock(&lock);
    return NULL;
}

void *writer(void *arg) {
    int i;
	rwlock_acquire_writelock(&lock);
	value++; //value gets "written" only by a writing thread
    wasteTime();
	printf("Finished writing value: %d\n", value);
	rwlock_release_writelock(&lock);
    return NULL;
}

int main(){
    printf("********** Readers Writers Simulation **********\n");
    
    int num_threads = 10;
    int bufferLength = 32;
    char buffer[bufferLength];

    FILE* myFile;
    if((myFile = fopen("scenarios.txt", "r")) == NULL){
        printf("error opening file");
    }
        
    //get input from file line by line - each line being a test case
    while(fgets(buffer, bufferLength, myFile) != NULL){
        value = 0; //set global read/write tracking variables back to 0
        printf("Current Test Case: %s\n", buffer);

        pthread_t threads[num_threads];
        rwlock_init(&lock);
        int i = 0;
        char rw = buffer[0];

        //for each character in the line, create a reading thread or a writing thread
        while(rw != '\n'){
            rw = buffer[i];
            if(rw == 'r'){
                pthread_create(&threads[i], NULL, reader, NULL);
            }
            else if(rw == 'w'){
                pthread_create(&threads[i], NULL, writer, NULL);
            }
            i++;
        }
        //before starting next test make sure all threads are finished
        for (i = 0; i < num_threads; i++){
	        pthread_join(threads[i], NULL);
        }
    }
    

    fclose(myFile);
    return 0;
}