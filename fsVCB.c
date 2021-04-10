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

    // Error condition to determine if any of the parameters we set for our VCB
    // are not greater than 0 
    if(!(aVCB_ptr->numberOfBlocks >= 0) || !(aVCB_ptr->sizeOfBlock >= 0) || !(aVCB_ptr->magicNumber >= 0)) {
        return 0;
    }
    else {
        return -1;
    }
}

// Load our VCB into the LBA at logical block 0
int loadVCB(VCB * aVCB_ptr) {

    aVCB_ptr = malloc(MINBLOCKSIZE);

    return -1;
}