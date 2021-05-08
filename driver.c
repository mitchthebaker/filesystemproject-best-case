/**************************************************************
* Class:  CSC-415
* Name: Mitchel Baker
* Student ID: 917679066
* Project: Basic File System
*
* File: driver.c
*
* Description: This is driver program for the file system; the 
*              main function below initializes the file system
*              and temporary storage for the VCB, then starts
*              the shell program in terminal. Once it starts 
*              commands can be typed into the prompt.
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

    // 1st argument: volume file
    // 2nd argument: volume size
    // 3rd argument: block size 
    char * filename;
	uint64_t volumeSize;
	uint64_t blockSize;

    // Total args should be 4
    if (argc >= 3) {
		
        filename = argv[1];
		volumeSize = atoll (argv[2]);
		blockSize = atoll (argv[3]);
	}

    // Have user rerun the program because they've inputted incorrect arguments
	else {
		
        printf ("Usage: ./driver volumeFileName volumeSize blockSize\n");
		return -1;
	}

    // Start up the partition system
    int retVal = startPartitionSystem (filename, &volumeSize, &blockSize);	
	printf("Opened %s, Volume Size: %llu;  BlockSize: %llu; Return %d\n", filename, (ull_t)volumeSize, (ull_t)blockSize, retVal);

    // Initialize the file system
    fs_init(volumeSize, blockSize);

    // Initialize temporary storage for vcb
    temp_init();

    // Start the shell for the user
    start_shell();
    
    return 0;
}