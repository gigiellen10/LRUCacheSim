#include "header.h"

int main(int argc, char* argv[]) {
    int c = 0, 
        verbose = FALSE,
        indexBits = 0,
        assoc = 1,
        blockOffset = 0;
    char fileString[100] = "";

// parse command line args to get cache parameters
    while( (c = getopt(argc,argv,"s:E:b:t:vh")) != -1){
        switch(c) {
            case 'h':
                printUsageInfo();
                exit(0);
            case 'v':
                // verbose flag
                verbose = TRUE;
                break;
            case 's':
                // opt arg will give the set index bits
                indexBits = atoi(optarg);
                break;
            case 'E':
                // optarg will give the associativity
                indexBits = atoi(optarg);
                break;
            case 'b':
                // optarg will give the block offset bits
                indexBits = atoi(optarg);
                break;
            case 't':
                // optarg will give the tracefile to open
                strcpy(fileString, optarg);
                break;
            default:
                printUsageInfo();
                exit(1);
        }
    }

    /* open and parse traces file to run simulation */
    FILE* traceFile = fopen(fileString, "r");

    if (traceFile != NULL) {
        // we opened file successfully
        runSim(traceFile, verbose, indexBits, assoc, blockOffset);
    } else {
        printf("The trace file \"%s\" could not be opened.\n", fileString);
        exit(1);
    }
    
    return 0;
}