#include "header.h"

/* main cach simulation function definitions */

/* precondition: traceFile is open
    This function runs the main caching simulation
*/
void runSim(FILE* traceFile, int verbose, int indexBits, int blockOffset) {
    char buffer[100] = "";
    
    int hits = 0,
        misses = 0,
        evictions = 0;

    // declare and initialize cache obj
    Cache cache;
    initCache(&cache);

    // parse valgrind trace file
    while ( fgets(buffer, sizeof(buffer), traceFile) ) {
        char* token = strtok(buffer, " ");
    
        // we don't want to consider instruction fetches
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

    deleteCache(&cache); // free cache mem
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

/* 
    precondition: associativity (ASSOC) and NUM_SETS must have been computed in another 
    function before calling this function.

    inits the cache object to empty sets and lines
*/
void initCache(Cache* cache) {

    cache->LRUCounter = 0;
    cache->sets = malloc(sizeof(Set) * NUM_SETS);
    
    // create empty set and fill with lines
    Line* lines = malloc(sizeof(Line) * ASSOC);
    Set* s;

    // init array of lines to empty
    for (int i = 0; i < ASSOC; i++) {
        lines[i].lastAccessed = 0;
        lines[i].validBit = 0;
        lines[i].tagBits = 0; // 0 for now, when filled will have actual data
    }

    // put empty lines into the set
    s->lines = lines;

    for (int i = 0; i < NUM_SETS; i++) {
        cache->sets[i] = *s; // add empty initialized sets to the cache
    }
}


// double check this function is right
void deleteCache(Cache* cache) {
    
    // free lines first
    for (int i = 0; i < ASSOC; i++) {
        free(cache->sets[i].lines);
    }

    // free array of sets
    for (int i = 0; i < NUM_SETS; i++) {
        free(cache->sets);
    }
}