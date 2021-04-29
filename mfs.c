/**************************************************************
* Class:  CSC-415
* Name: tania nemeth & team
* Student ID: 920680290
* Project: Basic File System
*
* File: mfs.c
*
* Description: 
*	
*	
*	
*
**************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "mfs.h"
#include "fsLow.h"
#include "fsVCB.h"
#include "bitmap.h"
ino_t curDir = 0;
ino_t rootDir = 0;
uint64_t volumeSize;
uint64_t blockSize;
//maybe initizlize fs_diriteminfo as null ?

//setup 
int fs_init(){
    // 'startPartitionSystem()' takes parameters:
    // 
    //  char * filename, 
    //  uint64_t * volSize, 
    //  uint64_t * blockSize
    uint64_t volumeSize = 10000000;
    uint64_t blockSize = 512;
    int fs_init_success = -1;

    //file writing into, block size, etc.,
    startPartitionSystem("fsVolume", &volumeSize, &blockSize);
   
    //Initialize our VCB struct, allocate enough memory to account for all its parameters
	VCB * aVCB_ptr = malloc(sizeof(* aVCB_ptr));

    // Error testing to determine whether VCB has been formatted or not 
	//
	// If myVCB_Ptr's 'magicNumber' is initialized, then our VCB has been formatted 
	if(aVCB_ptr->magicNumber == MAGIC_NUMBER) {

		printf("VCB initialized.\n");
	}

	// Otherwise, VCB is not initialized. Run initVCB() and loadVCB()
    //run init root directory 
	else {
		
		// Initialize the parameters for our VCB structure and load into LBA
        fs_init_success = (initVCB(aVCB_ptr, volumeSize, blockSize) == 0) ? 0 : fs_init_success; 
		fs_init_success = (loadVCB(aVCB_ptr) == 0) ? 0 : fs_init_success;

        // Create bitmap and then initialize it
        struct bitmap_t * aBitmap = create_bitmap(volumeSize, blockSize);

        // Calculate total number of blocks bits in terms of bytes
        uint64_t totalNumBytes = ceil((double) aBitmap->numberOfBlocks / 8);
        //printf("total number of bytes: %ld\n", totalNumBytes);

        // Get the size of the bitmap, in terms of blocks, by dividing total bytes by the block size
        uint64_t sizeOfBitmap = ceil((double) totalNumBytes / blockSize);
        //printf("size of bitmap: %ld\n", sizeOfBitmap);

        // Allocate memory for bitmap, then write to LBA
        map_init(aVCB_ptr, aBitmap, sizeOfBitmap, totalNumBytes);

		//initialize directory here?
		//something like new directory = initDirectory(parent) --> should be null for root
		int params_set_dir = initRootDir(aVCB_ptr, sizeOfBitmap);
	}
    
    free (aVCB_ptr);

    return fs_init_success;
}


int fs_close(){
    closePartitionSystem();
}