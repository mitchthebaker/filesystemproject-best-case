/**************************************************************
* Class:  CSC-415
* Name: Mitchel Baker and Tania Nemeth
* Student ID: 917679066
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

#define VCB_TITLE_LENGTH 64
#define MAGIC_NUMBER 0x20214150

typedef struct VCB {
    char vcbTitle[VCB_TITLE_LENGTH];
    uint64_t numberOfBlocks;   // We need to know the total number of blocks (Volume size / block size) 
    uint64_t numberOfBlocksRemaining;
    uint64_t sizeOfBlock;      // We need to know the size of each block (512)
    int LBA_indexOf_rootDir;   // Important so we know where the root dir starts in the LBA 
    int LBA_indexOf_freeSpace; // We must know where the freespace begins in the LBA 
    int magicNumber;           // We use this number to determine if our VCB is initialized
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

//Tania
//initialize all of the parameters in the directory structure 
int initRootDir(VCB * aVCB_ptr, uint64_t sizeOfBitmap);

//function that will create and initizalize empty directory entries in memory 
d_entry * createAndInitDir(ino_t parent,  VCB * aVCB_ptr);
//end Tania 

#endif