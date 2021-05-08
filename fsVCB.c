/**************************************************************
* Class:  CSC-415
* Name: Mitchel Baker and Tania Nemeth
* Student ID: 917679066
* Project: Basic File System
*
* File: fsVCB.c
*
* Description: This is the implementation for writing our volume 
*              control block to the LBA, in addition to creating 
*              and initializing the root directory.              
*
**************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "bitmap.h"
#include "fsLow.h"
#include "fsVCB.h"
#include "fsDir.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>

// Initialize our VCB with its default parameters.
//
// Expand upon the VCB and add any new parameters in 'fsVCB.h' if necessary 
int initVCB(VCB * aVCB_ptr, uint64_t volumeSize, uint64_t blockSize) {

    // Copy the title into the VCB
    char title[] = "Best Case - File System Project";
    strcpy(aVCB_ptr->vcbTitle, title);

    // Divide volumeSize by blockSize to get the total number of blocks
    aVCB_ptr->numberOfBlocks = volumeSize / blockSize;

    // Size per block, specified when starting up the file system (argv[3])
    aVCB_ptr->sizeOfBlock = blockSize;

    // Initialize the VCB's magic number to 0x20214150
    aVCB_ptr->magicNumber = MAGIC_NUMBER;

    // Determine if any of the parameters we set for our VCB are not greater than 0
    // If true, then invalid values exist
    //
    // Return -1 if failed, otherwise return 0 (OK)
    return (!(aVCB_ptr->numberOfBlocks >= 0) || !(aVCB_ptr->sizeOfBlock >= 0) 
         || !(aVCB_ptr->magicNumber >= 0)) ? -1 : 0;
}

//Tania 
Directory * createAndInitDir (ino_t parent, VCB * aVCB_ptr, Directory * rootDir) {

    // Initialize root directory's name to '/'?
    strcpy(rootDir->name, "/");

    // Set root's parent to NULL since it has no parent
    rootDir->parent = NULL;

    // need to indicate that the entries are free and set the parent
    for(int i = 0; i < MAX_NUM_ENTRIES; i++) {
        rootDir->entries[i].parent = parent;
        rootDir->entries[i].d_free = true;
    }

    // Return the pointer to a Directory struct designating our root directory
    return rootDir;
}

uint64_t initRootDir(VCB * aVCB_ptr, Directory * rootDir, uint64_t sizeOfBitmap){

    // First and foremost, create and then initialize the root directory with basic attributes
    Directory * root = createAndInitDir(0, aVCB_ptr, rootDir);

    //location is LBA
    //size is # of dir entries * size of directory entry

    // Now that the root dir is initialized, get the total size of root, divide by the block size, + 1 
    // to get the total number of root blocks to allocate
    uint64_t rootBlocks = (sizeof(* root) / aVCB_ptr->sizeOfBlock) + 1;

    // Request the number of root blocks to allocate from freespace
    uint64_t rootPosition = requestFSBlocks(aVCB_ptr, rootBlocks);

    // Then allocate the freespace blocks for root
    uint64_t blocksAllocated = allocFSBlocks(aVCB_ptr, rootBlocks, rootPosition);

    // Set root directory size = 2 since we are initializing with '.' / '..' entries
    root->size = 2;

    //create directory entry for .
    strcpy(root->entries[0].d_name, ".");
    root->entries[0].d_free = false;
    //type of file is directory
    root->entries[0].d_type = 'd';
    root->entries[0].d_ino = rootPosition;
    root->entries[0].parent = rootPosition;

    //for .. if parent == null it's the LBA, otherwise: its the parent
    //if parent == null ? LBA : parent
    //size = parent (sizeof(parent)), type of file is directory
    //create directory entry for ..
    strcpy(root->entries[1].d_name, "..");
    root->entries[1].d_free = false;
    //type of file is directory
    root->entries[1].d_type = 'd';
    root->entries[1].d_ino = rootPosition;
    root->entries[1].parent = rootPosition;

    //write directory entries to the file system
    LBAwrite(root, rootBlocks, rootPosition);

    // After updating index of rootDir to 'LBA', write the changes to LBA
    aVCB_ptr->LBA_indexOf_rootDir = rootPosition;
    LBAwrite(aVCB_ptr, 1, 0);

    //https://www.thegeekstuff.com/2012/06/c-directory/

    return 0;
}
//end Tania

// Load our VCB into the LBA at logical block 0
int loadVCB(VCB * aVCB_ptr) {

    // Since we are writing the VCB to block 0, decrement 'numberOfBlocks' param by 1
    aVCB_ptr->numberOfBlocks--;

    // Write our VCB struct to logical block 0
    uint64_t blocksWritten = LBAwrite(aVCB_ptr, 1, 0);

    // 'blocksWritten' will be 1 if the write was successful, therefore return 0 (OK)
    // -1 will be returned if the write to disk failed
    return (blocksWritten != 1) ? -1 : 0;
}

// Get the VCB from LBA
VCB * getVCB(VCB * aVCB_ptr) {

    // Lets check if our VCB was written to the LBA
    LBAread(aVCB_ptr, 1, 0);

    // Return the VCB from LBA
    return aVCB_ptr;
}