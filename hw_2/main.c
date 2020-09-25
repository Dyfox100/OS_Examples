#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "file_utils.h"
void mode2(int readRate, int printRate) {
  int length = 0;
  int smpls = printRate / readRate;
  int cpuUsr[smpls]; int cpuSys[smpls]; int cpuIdl[smpls];
  int memTotal; int tmpMem; int memFree[smpls];
  int cpuUserStart = 0; int cpuSystemStart = 0; int cpuIdleStart = 0;
  int tmpUsr, tmpSys, tmpIdl;
  char** arrayStrings = parseProcFileForLine("meminfo", "MemTotal", &length);
  sscanf(*arrayStrings, "%*s  %d kb", &memTotal);
  free(arrayStrings);
  while (1) {
     arrayStrings = parseProcFileForLine("stat", "cpu ", &length);
    sscanf(*arrayStrings, "cpu  %d %*d %d %d", &cpuUserStart, &cpuSystemStart, &cpuIdleStart);
    free(arrayStrings);
    //printf("%d, %d, %d", cpuUserStart, cpuSystemStart, cpuIdleStart);
    length = 0;
    for (int i = 0; i < printRate/readRate; i++) {
      sleep(readRate);
      arrayStrings = parseProcFileForLine("stat", "cpu ", &length);
      sscanf(*arrayStrings, "cpu  %d %*d %d %d", &tmpUsr, &tmpSys, &tmpIdl);
      free(arrayStrings);
      arrayStrings = parseProcFileForLine("meminfo", "MemFree", &length);
      sscanf(*arrayStrings, "%*s  %d kb", &tmpMem);
      free(arrayStrings);
      memFree[i] = (100 * tmpMem) / memTotal;
      cpuUsr[i] = 100 * (tmpUsr - cpuUserStart);
      cpuUsr[i] /= (tmpUsr - cpuUserStart + tmpSys - cpuSystemStart + tmpIdl - cpuIdleStart);
      cpuSys[i] = 100 * (tmpSys - cpuSystemStart);
      cpuSys[i] /= (tmpUsr - cpuUserStart + tmpSys - cpuSystemStart + tmpIdl - cpuIdleStart);
      cpuIdl[i] = 100 * (tmpIdl - cpuIdleStart);
      cpuIdl[i] /= (tmpUsr - cpuUserStart + tmpSys - cpuSystemStart + tmpIdl - cpuIdleStart);
    }
    int sumUsr = 0; int sumIdl = 0; int sumSys = 0; int sumMem = 0;
    for (int i = 0; i < printRate/readRate; i++) {
      sumMem += memFree[i];
      sumUsr += cpuUsr[i];
      sumIdl += cpuIdl[i];
      sumSys += cpuSys[i];
    }
    printf("CPU STATS:\nUser Percentage: %d, System Percentage: %d, Idle Percentage: %d\n",
      sumUsr/(smpls), sumSys/(smpls), sumIdl/(smpls));
    printf("Average Percentage of Memory Free: %d\n", sumMem / smpls);
    fflush( stdout );
  }


}

void mode1() {
  int length = 0;
  char** arrayStrings = parseProcFileForLine("cpuinfo", "model name", &length);
  printf("CPU MODEL: \n%s\n", *arrayStrings);
  for (int i = 0; i < length; i++) {
    free(*(arrayStrings + i));
  }
  free(arrayStrings);

  length = 0;
  arrayStrings = parseProcFileForLine("version", "version", &length);
  printf("\n\nLINUX KERNEL VERSION INFO: \n%s\n", *arrayStrings);
  for (int i = 0; i < length; i++) {
    free(*(arrayStrings + i));
  }
  free(arrayStrings);

  length = 0;
  arrayStrings = parseProcFileForLine("meminfo", "MemTotal", &length);
  printf("\n\nTOTAL MEMORY: \n%s\n", *arrayStrings);
  for (int i = 0; i < length; i++) {
    free(*(arrayStrings + i));
  }
  free(arrayStrings);


  length = 0;
  arrayStrings = parseProcFileForLine("iomem", "System RAM", &length);
  printf("\n\nTHE REGIONS MAPPED TO SYSTEM MEM AND KERNEL ARE: \n");
  for (int i = 0; i < length; i++) {
    printf("%s\n", *(arrayStrings + i));
    free(*(arrayStrings + i));
  }
  free(arrayStrings);

  length = 0;
  arrayStrings = parseProcFileForLine("iomem", "Kernel code", &length);
  printf("%s\n", *arrayStrings);
  for (int i = 0; i < length; i++) {
    free(*(arrayStrings + i));
  }
  free(arrayStrings);

  length = 0;
  arrayStrings = parseProcFileForLine("iomem", "Kernel data", &length);
  printf("%s\n", *arrayStrings);
  for (int i = 0; i < length; i++) {
    free(*(arrayStrings + i));
  }
  free(arrayStrings);

  length = 0;
  arrayStrings = parseProcFileForLine("iomem", "Kernel bss", &length);
  printf("%s\n", *arrayStrings);
  for (int i = 0; i < length; i++) {
    free(*(arrayStrings + i));
  }
  free(arrayStrings);

  length = 0;
  arrayStrings = parseProcFileForLine("uptime", " ", &length);
  printf("\n\nTIME SINCE LAST REBOOT (FIRST VALUE, IN SECONDS): \n%s\n", *arrayStrings);
  for (int i = 0; i < length; i++) {
    free(*(arrayStrings + i));
  }
  free(arrayStrings);
}

int main(int argc, char *argv[]) {
  int readRate = 0;
  int printRate = 0;
  if (argc == 1) {
    mode1();
  }
  else if (argc == 3) {
    readRate = atoi(argv[1]);
    printRate = atoi(argv[2]);
    mode2(readRate, printRate);
  }
  return 0;
}
