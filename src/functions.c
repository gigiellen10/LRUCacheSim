#include "header.h"

/* main cach simulation function definitions */

/* precondition: traceFile is open
    This function runs the main caching simulation
*/
void runSim(FILE* traceFile, int verbose, int indexBits, int assoc, int blockOffset) {
    char buffer[100] = "";
    
    int hits = 0,
        misses = 0,
        evictions = 0;


    while ( fgets(buffer, sizeof(buffer), traceFile) ) {
        char* token = strtok(buffer, " ");
    
        if (strcmp(token, "I") == 0) {
            continue;
        }

        char* address = strtok(NULL, ",");
        char* memSize = strtok(NULL, "\n");

        // if in verbose mode, print out
        if (verbose) {
            // Print parsed values (you can store or process them as needed)
            printf("%s %s,%s ", token, address, memSize);
        }

        // call caching func here

        fputc('\n', stdout);
    }

    fclose(traceFile);
}

/* helper functions! */

void printUsageInfo() {
    printf("Run the program with the following [options] flags:\n");
    printf("  -h: Optional help flag that prints usage info\n");
    printf("  -v: Optional verbose flag that displays trace info\n");
    printf("  -s <s>: Number of set index bits (S = 2^s is the number of sets)\n");
    printf("  -E <E>: Associativity (number of lines per set)\n");
    printf("  -b <b>: Number of block bits (B = 2^b is the block size)\n");
    printf("  -t <tracefile>: Name of the valgrind trace to replay\n");
}