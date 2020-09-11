#include <stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdlib.h>

#define NUM_SHARED_INTS 20
#define MAX_STRING_SIZE 27


int main(int argc, char **argv){
    key_t key = 5678;
    int c1, c2;
    size_t size_shared_buff = MAX_STRING_SIZE; // it's a bunch of chars, so max size == actualy size
    int parent_pid = getpid();
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
    sleep(1);
    if (parent_pid == getpid()){
        printf("hit\n");
        
        char *shared, *string;
        int shared_mem_id = shmget(key, size_shared_buff, IPC_CREAT | 0666);
        printf("shmid: %d\n", shared_mem_id);
        shared = shmat(shared_mem_id, NULL, 0);
        if (shared == (void *) -1) {
            printf("shmat");
        }
        string = shared;
        for (char c = 'a'; c <= 'z'; c++)
            *string++ = c;
        *string = '\0';
        shmdt(shared);
    }
    if (c1 == 0){
        char *shared1, *string1, chr;
        int shared_mem_id = shmget(key, size_shared_buff, 0666);
        shared1 = shmat(shared_mem_id, NULL, 0);
        string1 = shared1;
        chr = *string1;
        while (chr != '\0') {
            printf("%c", chr);
            chr = *string1++;
        }
    }
        
    return 0;
    }