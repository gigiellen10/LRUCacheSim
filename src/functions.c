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
    long tag = 0,
         indexVal = 0;

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

        // convert address string into a long int for bit manipulation
        long addy = strtol(address, NULL, 16);

        // parse the address based on the # of index bits, block offset and tagBits size
        calculateBits(addy, &tag, &indexVal, indexBits, blockOffset);

        if (verbose) {
            // Print parsed values (you can store or process them as needed)
            fprintf(stdout, "\n %s %s,%s", token, address, memSize);
        }

        // calculate the memory accesses, hits, misses etc.
        accessCache(&cache, indexVal, tag, verbose, &misses, &hits, &evictions);

        if (strcmp(token, "M") == 0) {
            // will be a hit by default
            ++hits; 

            if (verbose) {
                printf(" hit");
            }
        }

        // update cache counter for LRU evictions
        cache.LRUCounter++;
        
        if (verbose) {
            printf("\n");
        }
        
    }

    // output final hit, miss, eviction stats
    outputStats(hits, misses, evictions, verbose);

    deleteCache(&cache); // free cache mem
    
    fclose(traceFile);
}

void printBinary(long number) {
    // Assuming a 32-bit integer
    for (int i = sizeof(long) * 8 - 1; i >= 0; i--) {
        // Extract the i-th bit using a bitwise AND and a shift
        int bit = (number >> i) & 1;
        printf("%d", bit);
    }
    
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
    accesses the cache and performs an operation depending if hit, miss or eviction. HME object
    returns the number of hits, misses and evictions for that access.
*/
void accessCache(Cache* cache, int index, int tag, int verbose, int* misses, int* hits, int* evictions) {
    int isHit = FALSE, i = 0;
    
    // Map the index to the set
    Set* s = &cache->sets[index]; // Use a pointer for direct access to the set

    // Traverse the lines in the set, looking for the tag
    for (; i < ASSOC; ++i) {
        if (s->lines[i].validBit && s->lines[i].tagBits == tag) {
            
            isHit = TRUE;
            s->lines[i].lastAccessed = cache->LRUCounter;
            ++(*hits);

            if (verbose) {
                printf(" hit");
            }
            break;
        }
    }
    
    if (!isHit) {
        ++(*misses);
        if (verbose) {
            printf(" miss");
        }

        // add data to cache and find line to replace
        int iToReplace = findReplaceLine(s);
        if (s->lines[iToReplace].validBit) {
            if (verbose) {
                printf(" eviction");
            }
            ++(*evictions); // we must evict the lru valid line
            
        }

        // insert the new data into the line we found
        addData(s, tag, iToReplace, cache->LRUCounter);
    }
}

int findReplaceLine(Set* s) {
    int lruIndex = -1, lruMin = INT_MAX;

    for (int i = 0; i < ASSOC; i++) {
        if (!s->lines[i].validBit) {
            return i; // found a non-valid (empty) line, no eviction needed
        }

        if (s->lines[i].lastAccessed < lruMin) {
            lruMin = s->lines[i].lastAccessed; // Update minimum LRU timestamp
            lruIndex = i;
        }
    }

    // we found the lru line to evict, if any
    return lruIndex;
}

void addData(Set* set, int tag, int i, int currTime) {

    Line newLine;
    newLine.lastAccessed = currTime;
    newLine.tagBits = tag;
    newLine.validBit = 1;

    set->lines[i] = newLine;
}


void initCache(Cache* cache) {
    cache->LRUCounter = 0;

    // allocate mem for NUM_SETS # of sets
    cache->sets = malloc(sizeof(Set) * NUM_SETS);

    for (int i = 0; i < NUM_SETS; i++) {
        // create mem for assoc # of lines within each set
        cache->sets[i].lines = malloc(sizeof(Line) * ASSOC);

        // make all lines empty (validBit = 0)
        for (int j = 0; j < ASSOC; j++) {
            cache->sets[i].lines[j].lastAccessed = 0;
            cache->sets[i].lines[j].validBit = 0;
            cache->sets[i].lines[j].tagBits = 0;
        }
    }
}

/* 
    prints out the hits, misses and evictions that occurred in the cache simulation
*/
void outputStats(int hits, int misses, int evictions, int verbose) {
    if (verbose) {
        printf("\n");
    }

    printf("hits: %d  misses: %d  evictions: %d\n", hits, misses, evictions);
}

/*
    Frees all dynamic memory associated with the cache struct
*/
void deleteCache(Cache* cache) {
    
    // free all lines before deleting the sets array
    for (int i = 0; i < NUM_SETS; i++) {
        free(cache->sets[i].lines);
    }

    // free array of sets
    free(cache->sets);
}


/* 
    parses the address into the correct values for the tag and indexVal, and returns
    these values via output parameter (long*). Uses values of indexBits and blockOffset
    to perform the necessary computations 
*/
void calculateBits(long address, long* tag, long* indexVal, int indexBits, int blockOffset) {
    long mask = ((1L << indexBits) - 1) << blockOffset;
    (*indexVal) = (address & mask) >> blockOffset;

    mask = ~((1L << (blockOffset + indexBits)) - 1);
    (*tag) = (address & mask) >> (blockOffset + indexBits);
}