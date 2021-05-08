/**************************************************************
* Class:  CSC-415
* Name: Mitchel Baker and Tania Nemeth
* Student ID: 917679066, 920680290 (respective to names listed)
* Project: Basic File System
*
* File: fsVCB.h
*
* Description: This is the header file for fsVCB.c; the functions 
*              declared below are necessary for loading and 
*              initializing our volume control block in the 
*              first block of the LBA.
*
**************************************************************/

#ifndef _FSVCB_H
#define _FSVCB_H
#include "fsLow.h"
#include "fsDir.h"
#include <sys/types.h>

#define VCB_TITLE_LENGTH 64

// Magic number declaration, use this to determine if the VCB has already been allocated or not.
#define MAGIC_NUMBER 0x20214150

typedef struct VCB {
    char vcbTitle[VCB_TITLE_LENGTH];  // Title for the VCB, something simple like 'Team Best Case File System Project'
    uint64_t numberOfBlocks;          // We need to know the total number of blocks (Volume size / block size) 
    uint64_t numberOfBlocksRemaining; // Total number of freespace blocks remaining (may not use)
    uint64_t sizeOfBlock;             // We need to know the size of each block (512)
    uint64_t freeSpaceBlocks;         // Keep track of the number of freespace blocks 
    uint64_t LBA_indexOf_rootDir;     // Important so we know where the root dir starts in the LBA 
    uint64_t LBA_indexOf_freeSpace;   // We must know where the freespace begins in the LBA 
    int magicNumber;                  // We use this number to determine if our VCB is initialized
    /*ino_t cwd;                      //index of current working directory*/
} VCB;

// The purpose of 'initVCB' is to initialize all of the parameters within
// the 'VCB' structure. 
//
// Return a value of 0 upon success, -1 otherwise. 
int initVCB(VCB * aVCB_ptr, uint64_t volumeSize, uint64_t blockSize);

// The purpose of 'loadVCB' is to load our 'VCB' structure into the LBA at block 0.
//
// Return a value of 0 upon success, -1 otherwise.
int loadVCB(VCB * aVCB_ptr);

// Get the VCB from LBA
VCB * getVCB(VCB * aVCB_ptr);

//Tania
//initialize all of the parameters in the directory structure 
uint64_t initRootDir(VCB * aVCB_ptr, Directory * rootDir, uint64_t sizeOfBitmap);

//function that will create and initizalize empty directory entries in memory 
Directory * createAndInitDir(ino_t parent, VCB * aVCB_ptr, Directory * rootDir);
//end Tania 

#endif