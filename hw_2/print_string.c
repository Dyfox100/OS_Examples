#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
char *original = "Hacking!";

int main() {
  printf("My PID is: %d\n", getpid());
  char* duplicate;
  duplicate = strdup(original);
  while (1) {
    printf("The string is: %s\n", duplicate);
    fflush(stdout);
    sleep(3);
  }
  return 0;
}
