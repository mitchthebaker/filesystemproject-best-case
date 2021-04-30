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
#include <string.h>
#include "mfs.h"
#include "fsLow.h"
#include "fsVCB.h"
#include "bitmap.h"
ino_t curDir = 0;
ino_t rootDir = 0;
uint64_t volumeSize;
uint64_t blockSize;
//maybe initizlize fs_diriteminfo as null ?

u_int64_t blocksizeCache;


u_int64_t getBytes(u_int64_t bytes) {
    return ((bytes / blocksizeCache) + 1) * blocksizeCache;
}

//setup 
int fs_init(){
    // 'startPartitionSystem()' takes parameters:
    // 
    //  char * filename, 
    //  uint64_t * volSize, 
    //  uint64_t * blockSize
    uint64_t volumeSize = 10000000;
    uint64_t blockSize = 512;
    blocksizeCache = blockSize;
    int fs_init_success = -1;

    //file writing into, block size, etc.,
    startPartitionSystem("fsVolume", &volumeSize, &blockSize);
   
    //Initialize our VCB struct, allocate enough memory to account for all its parameters
	VCB * aVCB_ptr = malloc(sizeof(* aVCB_ptr));

    // Initialize a Directory structure to represent the root directory
    Directory * root = malloc(sizeof(* root)); 

    // Error testing to determine whether VCB has been formatted or not 
	//
	// If myVCB_Ptr's 'magicNumber' is initialized, then our VCB has been formatted 
	if(aVCB_ptr->magicNumber == MAGIC_NUMBER) {

		printf("VCB initialized.\n");
	}

	// Otherwise, VCB is not initialized. Run initVCB() and loadVCB()
    // Run init root directory 
	else {
		
		// Initialize the parameters for our VCB structure and load into LBA
        fs_init_success = (initVCB(aVCB_ptr, volumeSize, blockSize) == 0) ? 0 : fs_init_success; 
		fs_init_success = (loadVCB(aVCB_ptr) == 0) ? 0 : fs_init_success;

        // Create bitmap and then initialize it
        //struct bitmap_t * aBitmap = create_bitmap(volumeSize, blockSize);

        // Calculate total number of blocks bits in terms of bytes
        //uint64_t totalNumBytes = ceil((double) aBitmap->numberOfBlocks / 8);
        //printf("total number of bytes: %ld\n", totalNumBytes);

        // Get the size of the bitmap, in terms of blocks, by dividing total bytes by the block size
        //uint64_t sizeOfBitmap = ceil((double) totalNumBytes / blockSize);
        //printf("size of bitmap: %ld\n", sizeOfBitmap);

        // Allocate memory for bitmap, then write to LBA
        //map_init(aVCB_ptr, aBitmap, sizeOfBitmap, totalNumBytes);
        int freeSpaceBlocksWritten = map_initialize(aVCB_ptr);
        
		//initialize directory here?
		//something like new directory = initDirectory(parent) --> should be null for root
		int params_set_dir = initRootDir(aVCB_ptr, root, freeSpaceBlocksWritten);
	}
    curDir = 6;
    rootDir = 6;

    free (aVCB_ptr);
    free (root);

    aVCB_ptr = NULL;
    root = NULL;

    return fs_init_success;
}

char * fs_getcwd(char *buf, size_t size) {

    struct VCB *myVCB = malloc(512);
    myVCB = getVCB(myVCB);

    Directory *curr = malloc(getBytes(sizeof(*curr)));
    // DEBUG prints
    // printf("currentDir: %ld\n", curDir);
    // printf("index of rootDir: %d\n", myVCB->LBA_indexOf_rootDir);

    // Old LBAread() call
    //LBAread(curr, (sizeof(Directory) / myVCB->sizeOfBlock) + 1, curDir);

    // New LBAread() updated with index of our root dir
    // Also printed out the current directory name and its parent
    LBAread(curr, 1, myVCB->LBA_indexOf_rootDir);
    // DEBUG prints
    // printf("current dir name: %s\n", curr->name);
    // printf("current dir parent: %ld\n", curr->parent);

    char **directoryNames = malloc(sizeof(char *) * 10);
    int count = 0;
    while (strcmp("/", curr->name) != 0) {
        directoryNames[count] = malloc(sizeof(char) * 256);
        strcpy(directoryNames[count], curr->name);
        strcat(directoryNames[count], "/");
        count++;
        LBAread(curr, (sizeof(Directory) / myVCB->sizeOfBlock) + 1, curr->parent);
    }
    
    directoryNames[count] = malloc(sizeof(char) * 256);
    strcpy(directoryNames[count], curr->name);
    char *fullPath = malloc(sizeof(*fullPath) * 512);

    fullPath[0] = '\0';

    for (int i = count; i >= 0; i--) {
        strcat(fullPath, directoryNames[i]);
        free(directoryNames[i]);
    }
    free(directoryNames);
    free(myVCB);
    free(curr);
    return fullPath;
}

int fs_setcwd(char *buf) {
    return 0;
    // struct VCB *myVCB = malloc(512);
    // loadVCB(myVCB);
    // Directory *curr;
    // if (buf[0] == '/') {
    //     printf("STARTING FROM ROOT");
    // } else {
    //     curr = malloc(getBytes(sizeof(*curr)));
    //     LBAread(curr, (sizeof(*curr) / myVCB->sizeOfBlock), curDir);
    //     struct d_entry *entries = malloc(sizeof(getBytes(*entries)) * curr->size);
    //     int entryBlockCount = (sizeof(struct d_entry) * curr->size) / myVCB->sizeOfBlock + 1;
    //     LBAread(entries, entryBlockCount, curr->)
    // }
}

int fs_close(){
    closePartitionSystem();
}