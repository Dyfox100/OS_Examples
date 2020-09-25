#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "file_utils.h"

void mode2(int readRate, int printRate) {
  int length = 0;
  long int smpls = printRate / readRate;
  long int cpuUsr[smpls + 1], cpuSys[smpls + 1], cpuIdl[smpls + 1];
  long int memTotal, tmpMem, memFree[smpls + 1];
  long int tmpDskR, tmpDskW, diskRW[smpls + 1];
  long int tmpCtxs, ctxSwitches[smpls + 1];
  long int tmpUsr, tmpSys, tmpIdl;
  long int dskRStart, dskWStart;
  long int tmpFrk, frks[smpls + 1];
  char** arrayStrings = parseProcFileForLine("meminfo", "MemTotal", &length);
  sscanf(*arrayStrings, "%*s  %ld kb", &memTotal);
  free(arrayStrings);
  ctxSwitches[0] = 0; cpuUsr[0] = 0; cpuSys[0] = 0; cpuIdl[0] = 0; diskRW[0] = 0;
  while (1) {
    arrayStrings = parseProcFileForLine("stat", "processes", &length);
    sscanf(*arrayStrings, "processes  %ld", frks);
    free(arrayStrings);
    arrayStrings = parseProcFileForLine("stat", "ctxt", &length);
    sscanf(*arrayStrings, "ctxt  %ld", ctxSwitches);
    free(arrayStrings);
    arrayStrings = parseProcFileForLine("stat", "cpu ", &length);
    sscanf(*arrayStrings, "cpu  %ld %*d %ld %ld", cpuUsr, cpuSys, cpuIdl);
    free(arrayStrings);
    arrayStrings = parseProcFileForLine("diskstats", "sda1", &length);
    sscanf(*arrayStrings, "%*d %*d %*s %*d %*d %ld %*d %*d %*d %ld ", &dskRStart, &dskWStart);
    free(arrayStrings);
    diskRW[0] = dskRStart + dskWStart;
    //printf("%lu disk start\n", diskRW[0]);
    for (int i = 1; i <= smpls; i++) {
      sleep(readRate);
      arrayStrings = parseProcFileForLine("stat", "processes", &length);
      sscanf(*arrayStrings, "processes  %ld", &tmpFrk);
      free(arrayStrings);
      arrayStrings = parseProcFileForLine("stat", "ctxt", &length);
      sscanf(*arrayStrings, "ctxt  %ld", &tmpCtxs);
      free(arrayStrings);
      arrayStrings = parseProcFileForLine("diskstats", "sda1", &length);
      sscanf(*arrayStrings, "%*d %*d %*s %*d %*d %ld %*d %*d %*d %ld ", &tmpDskR, &tmpDskW);
      //sprintf("%lu, %lu disk read write %lu\n", tmpDskR, tmpDskW, (tmpDskR + tmpDskW - diskRW[i - 1]));
      arrayStrings = parseProcFileForLine("stat", "cpu ", &length);
      sscanf(*arrayStrings, "cpu  %ld %*d %ld %ld", &tmpUsr, &tmpSys, &tmpIdl);
      free(arrayStrings);
      arrayStrings = parseProcFileForLine("meminfo", "MemFree", &length);
      sscanf(*arrayStrings, "%*s  %ld kb", &tmpMem);
      free(arrayStrings);
      frks[i] = (tmpFrk - (frks[i-1] * (i != 1)) - frks[0]);
      ctxSwitches[i] = (tmpCtxs - (ctxSwitches[i -1] * (i != 1)) - ctxSwitches[0]);
      diskRW[i] = (tmpDskR + tmpDskW - (diskRW[i-1] * (i != 1)) - diskRW[0]);
      memFree[i] = (100 * tmpMem) / memTotal;
      cpuUsr[i] = tmpUsr - (cpuUsr[i-1] * (i != 1)) - cpuUsr[0];
      cpuSys[i] = tmpSys - (cpuSys[i-1] * (i != 1)) - cpuSys[0];
      cpuIdl[i] = tmpIdl - (cpuIdl[i-1] * (i != 1)) - cpuIdl[0];
    }
    long int sumUsr = 0, sumIdl = 0, sumSys = 0, sumMem = 0, sumDskRW = 0, sumCtx = 0, sumFrks = 0;
    for (int i = 1; i <= smpls; i++) {
      sumFrks += frks[i] / readRate;
      sumCtx += ctxSwitches[i] / readRate;
      sumDskRW += diskRW[i] / readRate;
      sumMem += memFree[i];
      sumUsr += (100 * cpuUsr[i]) / (cpuUsr[i] + cpuSys[i] + cpuIdl[i]);
      sumIdl += (100 * cpuIdl[i]) / (cpuUsr[i] + cpuSys[i] + cpuIdl[i]);
      sumSys += (100 * cpuSys[i]) / (cpuUsr[i] + cpuSys[i] + cpuIdl[i]);
    }
    printf("CPU STATS:\nUser Percentage: %ld, System Percentage: %ld, Idle Percentage: %ld\n",
      sumUsr/(smpls), sumSys/(smpls), sumIdl/(smpls));
    printf("Average Percentage of Memory Free: %ld\n", sumMem / smpls);
    printf("Average Disk Sectors Read And Written Per Second: %ld\n", sumDskRW / smpls);
    printf("Average Context Switches Per Second: %ld\n", sumCtx / smpls);
    printf("Average Processes Created Per Second: %ld\n", sumFrks / smpls);
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
