#include "header.h"

/* main cach simulation function definitions */

/* precondition: traceFile is open
    This function runs the main caching simulation
*/
void runSim(FILE* traceFile, int verbose, int indexBits, int assoc, int blockOffset) {
    char buffer[100] = "";
    
    // int hits = 0,
    //     misses = 0,
    //     evictions = 0;


    while ( fgets(buffer, sizeof(buffer), traceFile) ) {
        char* token = strtok(buffer, " "); 

        // skip 'I' entries in valgrind file
        if (strcmp(token, "I") == 0) {
            continue; 
        }

        // assume the valgrind files are well formatted
        char* accessType = strtok(NULL, " ");
        char* address = strtok(NULL, ",");
        char* memSize = strtok(NULL, "\n");

        // if we are in verbose mode, print out the above info
        if (verbose) {
            printf("%s %s,%s ", accessType, address, memSize);
        }

        // call miss/hit calculation functions here

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