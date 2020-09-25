#include <stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define MAX_STRING_SIZE 50


int main(int argc, char **argv){
    //printf("child starting exec");
    int pipe_handle[2];
    char *shared, *string1, *base_string;
    base_string = malloc(MAX_STRING_SIZE);
    key_t shared_mem_key;
    int child_number;
    pipe_handle[0] = atoi(argv[2]);
    pipe_handle[1] = atoi(argv[3]);
    child_number = atoi(argv[4]);
    strcpy(base_string, argv[1]);

    read(pipe_handle[0], (char *)&shared_mem_key, sizeof(key_t));
    int shared_mem_id = shmget(shared_mem_key, MAX_STRING_SIZE, 0666);

    sem_t *sem_out, *sem_in;
    if (child_number == 1) {
        sem_in = sem_open("/p_to_c_1", 0);
        sem_out = sem_open("/c_1_to_c_2", 0);
    }
    else if (child_number == 2) {
        sem_in = sem_open("/c_1_to_c_2", 0);
        sem_out = sem_open("/c_2_to_p", 0);
    }

    char *int_string;
    int_string = malloc(4);
    int count = 0;

    //seed rand off of time so all processes don't print same random numbers.
    srand((int)time(NULL) + child_number);

    for (int i = 0; i < 100; i++) {


        sem_wait(sem_in);
        shared = shmat(shared_mem_id, NULL, 0);
        string1 = shared;
        //sleep(1);
        while (*string1 != '\0') {
            printf("%c", *string1);
            *string1 = '\0';
            string1++;
            count++;
        }
        printf("\n");
        string1 -= count;
        count = 0;
        
        while(*base_string != '\0') {
            *string1 = *base_string;
            string1++;
            base_string++;
            count ++;
        }
        base_string -= count;
        count = 0;

        snprintf(int_string, 4, "%d", rand() % 1000);
        while(*int_string != '\0') {
            *string1 = *int_string;
            string1++;
            int_string++;

        }
        *string1 = '\0';

        shmdt(shared);
        sem_post(sem_out);
    }
    sem_close(sem_in);
    sem_close(sem_out);
    return 0;
}