#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include<sys/wait.h>

int main () {
  printf("This Proc's PID: %d", getpid());
  fflush(stdout);
  while(1) {
    sleep(10);
  }
  return 0;
}
