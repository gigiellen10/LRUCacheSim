#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

// global cache variables
extern int ASSOC; // # lines
extern int NUM_SETS; // sets in the cache

/* cache struct declarations */

typedef struct line {
    int validBit; // is the line valid data
    unsigned long int tagBits;
    int lastAccessed; // timestamp of last accessed (for LRU algorithm)
} Line;

typedef struct set {
    Line* lines; // # lines = associativity
} Set;

typedef struct cache {
    Set* sets; // # sets = 2^s where s is the # of set index bits
    int LRUCounter; // keep track of the "time" to find LRU line
} Cache;


/* function prototypes */
void printUsageInfo();
void runSim(FILE* traceFile, int verbose, int indexBits, int blockOffset);
void initCache(Cache* cache);
void deleteCache(Cache* cache);
