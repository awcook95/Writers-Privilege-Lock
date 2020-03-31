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

void reading_writing(){ //to simulate reading/writing just waste some time
    int x = 0, i, j, T;
    T = rand()%10000;
    for(i = 0; i < T; i++){
        for(j = 0; j < T; j++){
            x = i * j;
        }
    }
}

int main(){
    int bufferLength = 32;
    char buffer[bufferLength];
    FILE* myFile = fopen("scenarios.txt ","r");
    if(ptr = NULL){
        printf("failed to open file\n");
        return 0;
    }

    while(fgets(buffer, bufferLength, myFile)){
        /*
        int i = 0;
        char rw = buffer[i];
        while(rw != '\n'){
            if(rw == 'r')
            //do something

            else
            //do something else
        }
        */
        printf("%s\n", buffer);
    }

    fclose(myFile);
}