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

        // if in verbose mode, print out
        if (verbose) {
            // Print parsed values (you can store or process them as needed)
            printf("\n %s %s,%s ", token, address, memSize);
        }

        // convert address string into a long int for bit manipulation
        long addy = strtol(address, NULL, 16);

        // parse the address based on the # of index bits, block offset and tagBits size
        calculateBits(addy, &tag, &indexVal, indexBits, blockOffset);
        printf("\n address: ");
        printBinary(addy);
        printf(", index: ");
        printBinary(indexVal);
        printf(", tag: ");
        printBinary(tag);
        printf("\n");

        // calculate the memory accesses, hits, misses etc.
        //accessCache(&cache, indexVal, tag, verbose, &misses, &hits, &evictions);

        if (strcmp(token, "M") == 0) {
            // second access for modify = load + store
            //accessCache(&cache, indexVal, tag, verbose, &misses, &hits, &evictions);
        }

        // update cache counter for LRU evictions
        cache.LRUCounter++;
    }

    // output final hit, miss, eviction stats
    outputStats(hits, misses, evictions);

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

void accessCache(Cache* cache, int index, int tag, int verbose, int* misses, int* hits, int* evictions) {
    int isHit = FALSE,
        fullLines = 0,
        i = 0;

    // map the index to the set
    Set s = cache->sets[index];

    // traverse the lines in the set, looking for the tag
    for (; i < ASSOC; ++i) {
        if (s.lines[i].tagBits == tag && s.lines[i].validBit) {
            // the data is in the cache!
            isHit = TRUE;
            break;
        } else if (s.lines[i].validBit) {
            // increment the # valid lines count, used to see if set is full (we need to evict if yes)
            ++fullLines;
        } else {
            // we found an empty slot to store the data
            break;
        }
    }

    // all lines were full, evict LRU line
    if (fullLines == ASSOC) {
        // we need to evict a line!
        i = evictLine(cache, index);

        if (verbose) {
            printf(" eviction");
        }
    }

    // if we never got a hit, increment miss count and print the results
    if (!isHit) {
        ++(*misses);

        // bring data into the cache
        addData(cache, index, tag, i, cache->LRUCounter);
        
        if (verbose) {
            printf(" miss");
        }
    } else {
        ++(*hits);

        if (verbose) {
            printf(" hit");
        }
    }

    if (verbose) {
        fputc('\n', stdout); // for proper formatting
    }
    

    // store modified set back into the cache
    cache->sets[index] = s;
}

/* 
    precondition: if an eviction was necessary, the LRU line was evicted and i corresponds to an empty spot
    add the data to the cache at the set specified by index, and line i within the set
*/
void addData(Cache* cache, int index, int tag, int i, int currTime) {
    Line newLine;
    newLine.lastAccessed = currTime;
    newLine.tagBits = tag;
    newLine.validBit = 1;

    // add to cache
    cache->sets[index].lines[i] = newLine;
}

/*
    Searches the cache and evicts the least recently used line in the cache.
    The function returns the line index at which the LRU data was found and evicted, so 
    new data can replace it in successive operations.
*/
int evictLine(Cache* cache, int index) {
    int lruIndex = -1,
        lruMin = INT_MAX;

    for (int i = 0; i < ASSOC; i++) {
        // search the set at the specified index for the LRU line
        if (cache->sets[index].lines[i].lastAccessed < lruMin) {
            lruIndex = i;
        }
    }

    // evict line at lruIndex, which will be overwritten
    return lruIndex;
}

/* 
    precondition: associativity (ASSOC) and NUM_SETS must have been computed in another 
    function before calling this function.

    inits the cache object to empty sets and lines
*/
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
void outputStats(int hits, int misses, int evictions) {
    printf("\nhits: %d  misses: %d  evictions: %d\n", hits, misses, evictions);
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
    // // get index bits
    // int mask = (1 << indexBits) - 1;
    // *indexVal = (address >> blockOffset) & mask;

    // // get remaining tag bits after factoring in the block offset and index bits
    // *tag = address >> (blockOffset + indexBits);
}