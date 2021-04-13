/**************************************************************
* Class:  CSC-415
* Name: Mitchel Baker 
* Student ID: 917679066
* Project: Basic File System
*
* File: fsVCB.c
*
* Description: This is the implementation for writing our volume 
*              control block to the LBA.
*
**************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "fsLow.h"
#include "fsVCB.h"
#include "fsDir.h"

// Initialize our VCB with its default parameters.
//
// Expand upon the VCB and add any new parameters in 'fsVCB.h' if necessary 
int initVCB(VCB * aVCB_ptr, uint64_t volumeSize, uint64_t blockSize) {

    aVCB_ptr->numberOfBlocks = volumeSize / blockSize;
    aVCB_ptr->sizeOfBlock = blockSize;
    aVCB_ptr->magicNumber = MAGIC_NUMBER;

    // TODO's
    //

    // aVCB_ptr->LBA_indexOf_rootDir = index where root dir 
    // aVCB_ptr->LBA_indexOf_freeSpace = index of free space 

    // Determine if any of the parameters we set for our VCB are not greater than 0
    // If true, then invalid values exist
    //
    // Return -1 if failed, otherwise return 0 (OK)
    return (!(aVCB_ptr->numberOfBlocks >= 0) || !(aVCB_ptr->sizeOfBlock >= 0) 
         || !(aVCB_ptr->magicNumber >= 0)) ? -1 : 0;
}
int initRootDir(VCB * aVCB_ptr){
    //assumming VCB initialize for now 
    //need to get the # of directory entries and size of DE to figure out # of bytes to allocate
    //multiply and add (block size - 1) then / by block size to get the # of blocks 
    //aVCB_ptr->numberOfBlocks = 
    //populate with initizalied, default, empty directory entries (malloc and init array of DE's to default empty entries)
    Directory root;     //allocate space for root directory 
    

}
//LBA write of the blocks to volume 
int writeDir(VCB * aVCB_ptr){
    //LBA write blocks 
    //LBA write of the . & .. entries 
    //set VBA -> LBARootDir (using #2 ?)

}

// Load our VCB into the LBA at logical block 0
int loadVCB(VCB * aVCB_ptr) {

    // Allocate 512 bytes and read block 0 from LBA
    char * aBuffer = malloc(MINBLOCKSIZE);
    uint64_t retRead = LBAread(aBuffer, 1, 0);
    printf("\nBlock 0 of volume: %s\n", aBuffer);

    // Write our VCB struct to logical block 0
    uint64_t blocksWritten = LBAwrite(aVCB_ptr, 1, 0);

    // Lets check if our VCB was written to the LBA
    LBAread(aBuffer, 1, 0);

    printf("Block 0 of volume: %s\n", aBuffer);

    // Cleanup
    free(aBuffer);

    // 'blocksWritten' will be 1 if the write was successful, therefore return 0 (OK)
    // -1 will be returned if the write to disk failed
    return (blocksWritten != 1) ? -1 : 0;
}