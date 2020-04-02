#include "readerwriter.c"

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
        int num_reads = 0;
        int num_writes = 0;
        printf("Current Test Case: %s", buffer);

        pthread_t threads[num_threads];
        rwlock_init(&lock);
        int i = 0;
        char rw = buffer[0];

        //for each character in the line, create a reading thread or a writing thread
        while(rw == 'r' || rw == 'w'){
            rw = buffer[i];
            if(rw == 'r'){
                pthread_create(&threads[i], NULL, reader, NULL);
                num_reads++;
            }
            else if(rw == 'w'){
                pthread_create(&threads[i], NULL, writer, NULL);
                num_writes++;
            }
            i++;
        }
        //before starting next test make sure all threads are finished
        for (i = 0; i < num_threads; i++){
	        pthread_join(threads[i], NULL);
        }
        printf("Number of reads: %d\nNumber of writes: %d\n\n", num_reads, num_writes);
    }
    

    fclose(myFile);
    return 0;
}