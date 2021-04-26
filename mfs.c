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
#include "mfs.h"
#include "fsLow.h"
#include "fsVCB.h"
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
	else {
		
		// Initialize the parameters for our VCB structure and load into LBA
        fs_init_success = (initVCB(aVCB_ptr, volumeSize, blockSize) == 0) ? 0 : fs_init_success; 
		fs_init_success = (loadVCB(aVCB_ptr) == 0) ? 0 : fs_init_success;

		//initialize directory here?
		//something like new directory = initDirectory(parent) --> should be null for root
		int params_set_dir = initRootDir(aVCB_ptr);
		
		
	}
    
    //load into file system
    //loadVCB(aVCB_ptr);
    //write root directory into the file system
    //initRootDir(aVCB_ptr);
    free (aVCB_ptr);

    return fs_init_success;
}


int fs_close(){
    closePartitionSystem();
}