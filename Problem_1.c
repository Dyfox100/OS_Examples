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
    key_t key = 12345;
    char *shared, *string1;
    int c1, c2, status_child_proc, shared_mem_id;
    int parent_pid = getpid();

    int pipe_handle[2];
    pipe(pipe_handle);

    sem_t *sem_out = sem_open("/p_to_c_1", O_CREAT, 0666, 0);
    sem_t *sem_in = sem_open("/c_2_to_p", O_CREAT, 0666, 0);
    sem_t *sem_for_child = sem_open("/c_1_to_c_2", O_CREAT, 0666, 0);

    printf("Parent PID: %d\n", parent_pid);
    c1 = fork();
    if (c1 == -1) {
        printf("Error Creating Child 1");
    }
    else if (c1 == 0) {
        printf("first chid PID: %d\n", getpid());
    }
    else {
        c2 = fork();
        if (c2 == -1) {
            printf("Error Creating Child 2\n");
        }
        else if (c2 == 0) {
            printf("second child PID: %d\n", getpid());
        }
    }
    // If it's the parent, open shared memory segment, write key to pipe.
    if (parent_pid == getpid()){     
        
        shared_mem_id = shmget(key, MAX_STRING_SIZE, IPC_CREAT | 0666); 
        // initilizing shared mem region so it won't print random garbage on first loop.
        shared = shmat(shared_mem_id, NULL, 0);
        string1 = shared;
        *string1 = '\0'; 
        write(pipe_handle[1], (void *)&key, sizeof(key_t));
        write(pipe_handle[1], (void *)&key, sizeof(key_t));
        close(pipe_handle[0]);
    }
    // If not parent exec child w/ pipe handle and name passed as cmd args.
    else if  (c1 == 0) {
        char pipe_string1[20];
        char pipe_string2[20];
        snprintf(pipe_string1, 20, "%d", pipe_handle[0]);
        snprintf(pipe_string2, 20, "%d", pipe_handle[1]);
        execl("child", "child", "ChildOne: \0", pipe_string1, pipe_string2, "1", (char *)NULL);
    }
    else if (c2 == 0) {
        char pipe_string1[20];
        char pipe_string2[20];
        snprintf(pipe_string1, 20, "%d", pipe_handle[0]);
        snprintf(pipe_string2, 20, "%d", pipe_handle[1]);
        execl("child", "child", "ChildTwo: \0", pipe_string1, pipe_string2, "2", (char *)NULL);
    }
    // Only Parent executing from here out:
    // Loop 100 times and put info in shared mem
    // Get semaphore before each time.
    char *int_string;
    int_string = malloc(4);
    char *base_string = "Parent: \0";
    
    sem_post(sem_in);
    int count = 0;

    //seed rand off of time so all processes don't print same random numbers.
    srand((int)time(NULL));

    for (int i = 0; i < 101; i++) {
        // read from shared mem and print
        sem_wait(sem_in);
        shared = shmat(shared_mem_id, NULL, 0);
        string1 = shared;
        while (*string1 != '\0') {
            printf("%c", *string1);
            *string1 = '\0';
            string1++;
            count++;
        }
        printf("\n");
        string1 -= count;
        count = 0;
        // overwrite data with new message
        while(*base_string != '\0') {
            *string1 = *base_string;
            string1++;
            base_string++;
            count ++;
        }
        base_string -= count;
        count = 0;
        // get random int and add to buffer
        snprintf(int_string, 4, "%d", (rand() % 1000) + 1);
        while(*int_string != '\0') {
            *string1 = *int_string;
            string1++;
            int_string++;
        }
        *string1 = '\0';
        shmdt(shared);
        sem_post(sem_out);
    }
    wait(&status_child_proc);
    wait(&status_child_proc);
    //clean up semaphores and shared mem after other processes finish
    sem_close(sem_in);
    sem_close(sem_out);
    sem_close(sem_for_child);

    sem_unlink("/p_to_c_1");
    sem_unlink("/c_2_to_p");
    sem_unlink("/c_1_to_c_2");

    shmctl(shared_mem_id, IPC_RMID, 0);

    return 0;
    }