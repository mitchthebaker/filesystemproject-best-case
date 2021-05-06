/**************************************************************
* Class:  CSC-415
* Name: Team Best Case
* Student ID: 917679066
* Project: Basic File System
*
* File: fsshell.h
*
* Description: This is the header file for fsshell.c; the function
*              below starts the shell program in terminal.               
*              Once it starts commands can be typed into the prompt.
*
**************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <math.h>
#include <time.h>

#include "mfs.h"
#include "fsLow.h"
#include "fsVCB.h"
#include "bitmap.h"
#include "fsDir.h"
#include "fsshell.h"

// Main program for running the file system
int main (int argc, char * argv[]) {

    char * filename;
	uint64_t volumeSize;
	uint64_t blockSize;

    if (argc > 3) {
		
        filename = argv[1];
		volumeSize = atoll (argv[2]);
		blockSize = atoll (argv[3]);
	}
	else {
		
        printf ("Usage: ./driver volumeFileName volumeSize blockSize\n");
		return -1;
	}

    int retVal = startPartitionSystem (filename, &volumeSize, &blockSize);	
	printf("Opened %s, Volume Size: %llu;  BlockSize: %llu; Return %d\n", filename, (ull_t)volumeSize, (ull_t)blockSize, retVal);

    temp_init();
    fs_init(volumeSize, blockSize);
    start_shell();
    
    return 0;
}