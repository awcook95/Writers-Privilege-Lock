#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct _rwlock{
    sem_t rmutex; //mutual exclusion inside readers function
    sem_t wmutex; //likewise but for writers
    sem_t readTry; //lock used by both to coordinate writing priority
    sem_t resource; //locks the resource. Mutually excludes 1 writer and many readers
    int readers, writers; //count to track # of readers and writers
} rwlock_t;

void rwlock_init(rwlock_t *rw){
    rw->readers = 0;
    rw->writers = 0;
    sem_init(&rw->rmutex, 0, 1);
    sem_init(&rw->wmutex, 0, 1);
    sem_init(&rw->readTry, 0, 1);
    sem_init(&rw->resource, 0, 1);
}

void rwlock_acquire_readlock(rwlock_t *rw){
    //readTry acts as a gate that can be closed by a writer before a reader can increment the count and start reading
    //when a writer closes this gate, no additional readers can starve the writer. The readers still reading will finish.
    sem_wait(&rw->readTry); 
    sem_wait(&rw->rmutex); //once the gate is passed, this is a standard mutual exclusion
    rw->readers++;
        if(rw->readers == 1){
            sem_wait(&rw->resource); //first reader locks the resource
        }
    sem_post(&rw->rmutex);
    sem_post(&rw->readTry);
}

void rwlock_release_readlock(rwlock_t *rw){
    sem_wait(&rw->rmutex);
    rw->readers--;
        if(rw->readers == 0){
            sem_post(&rw->resource); //last reader out unlocks the resource
        }
    sem_post(&rw->rmutex);
}

void rwlock_acquire_writelock(rwlock_t *rw){
    sem_wait(&rw->wmutex);
    rw->writers++;
        if(rw->writers == 1){
            sem_wait(&rw->readTry); //first writer blocks any additional readers from queuing
        }
    sem_post(&rw->wmutex);
    sem_wait(&rw->resource); //one writer allowed at a time
}

void rwlock_release_writelock(rwlock_t *rw){
    sem_post(&rw->resource); //writer finishes
    sem_wait(&rw->wmutex);
    rw->writers--;
        if(rw->writers == 0){
            sem_post(&rw->readTry); //last writer to finish opens the gate for readers
        }
    sem_post(&rw->wmutex);
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

//threads will run these functions
void *reader(void *arg) {
    int i;
	rwlock_acquire_readlock(&lock);
    wasteTime();
	printf("Reading value: %d\n", value);
	rwlock_release_readlock(&lock);
    return NULL;
}

void *writer(void *arg) {
    int i;
	rwlock_acquire_writelock(&lock);
	value++; //value gets "written" only by a writing thread
    wasteTime();
	printf("Writing value: %d\n", value);
	rwlock_release_writelock(&lock);
    return NULL;
}