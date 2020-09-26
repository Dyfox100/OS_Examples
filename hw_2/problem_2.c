#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include<sys/wait.h>

#include "file_utils.h"


int main() {
  int parent_pid = getpid(), c1 = 0;
  int *status = 0;
  printf("Parent PID: %d\n", parent_pid);
  c1 = fork();
  if (c1 == -1) {
      printf("Error Creating Child 1");
  }
  else if (c1 == 0) {
      int error = execl("child", "child", (char *)NULL);
      printf("Error Creating Child: %d", error);
      fflush(stdout);
  }
  while (1) {
    sleep(10);
  }
  wait(status);
  return 0;
}
