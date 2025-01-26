#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

void printUsageInfo();
void runSim(FILE* traceFile, int verbose, int indexBits, int assoc, int blockOffset);
