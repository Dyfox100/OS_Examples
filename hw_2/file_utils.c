#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "file_utils.h"

char** parseProcFileForLine(char fileName[], char lineToGet[], int* length) {
  FILE* filePointer = NULL;
  char* line = NULL;
  char** returnArray = malloc(sizeof(char**));
  size_t len = 0;
  ssize_t read = 0;
  int countAdded = 0;

  char* procFS = "/proc/";
  char* fullPath = malloc(strlen(fileName) + strlen(procFS) + 1);
  strcpy(fullPath, procFS);
  strcat(fullPath, fileName);


  filePointer = fopen(fullPath, "r");
  if (filePointer== NULL) {
    printf("Error opening file, maybe not root?");
  }

  while ((read = getline(&line, &len, filePointer)) != -1) {
      // printf("Retrieved line of length %zu:\n", read);
      // printf("%s", line);
      line[strcspn(line, "\n")] = 0;
      if(strstr(line, lineToGet)) {
        if (countAdded != 0)
          returnArray = realloc(returnArray, sizeof(char**) * (countAdded + 1));
        *(returnArray + countAdded) = malloc((read + 1) * sizeof(char));
        strcpy(*(returnArray + countAdded), line);
        countAdded++;
      }
  }
  *length = countAdded;
  fclose(filePointer);
  if (line)
      free(line);
  free(fullPath);
  return returnArray;
}
