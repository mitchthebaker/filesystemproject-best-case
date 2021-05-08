/**************************************************************
* Class:  CSC-415
* Name: Mitchel Baker 
* Student ID: 917679066
* Project: Basic File System
*
* File: bitmap.c
*
* Description: This file allows for initializing a bitmap used for
*              freespace. It is also possible to allocate/deallocate,
*              and request freespace blocks whenever new directories
*              or files must be created.
*
**************************************************************/

#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> 
#include <stdbool.h>
#include "fsVCB.h"
#include "bitmap.h"
#include "fsLow.h"

// Initialize our map with initial values '1', which signifies that the blocks are free to use 
// Otherwise, blocks allocated with values of '0' signify that the blocks are allocated
int map_initialize(VCB * aVCB_ptr) {

    // Calculate the total number of freespace blocks
    uint64_t freeSpaceBlocks = (aVCB_ptr->numberOfBlocks / aVCB_ptr->sizeOfBlock) + 1; // +1 to account for rounding

    // Allocate memory for freespace
    uint8_t * freespace = malloc((sizeof(uint8_t) * aVCB_ptr->numberOfBlocks));

    // Iterate over the total number of blocks, and set each freespace block to 1
    // If the loop iterates past 'freeSpaceBlocks', then set these blocks to 0 since these are not for freespace
    int i;
    for(i = 0; i < aVCB_ptr->numberOfBlocks; i++) {

        // if 'i' is less than 'freesSpaceBlocks' then we know to set these to 1 since the freespace blocks
        // must already be allocated for the freespace itself
        if(i <= freeSpaceBlocks) {
            freespace[i] = 1;
        }
        
        // Set any remaining blocks in the freespace to 0, signifiying that they are free for future allocation
        else {
            freespace[i] = 0;
        }
    }

    // Initialize index of freespace in VCB, then update total number of freespace blocks
    // After, update VCB in LBA
    aVCB_ptr->LBA_indexOf_freeSpace = 1;
    aVCB_ptr->freeSpaceBlocks = freeSpaceBlocks;
    LBAwrite(aVCB_ptr, 1, 0);

    // Now allocate the freespace position into the LBA
    uint64_t blocksWritten = LBAwrite(freespace, aVCB_ptr->freeSpaceBlocks, aVCB_ptr->LBA_indexOf_freeSpace);

    // Free the memory allocated for total number of freespace blocks
    free(freespace);

    // Return the total number of freespace blocks written
    return blocksWritten;
}

// Allocate some amount of blocks from the LBA
uint64_t allocFSBlocks(VCB * aVCB_ptr, uint64_t numBlocksToAlloc, uint64_t blockPos) {

    printf("\n---------------------------------------\n");
    printf("numBlocksToAlloc: %ld.. at block pos %ld\n", numBlocksToAlloc, blockPos);
    printf("---------------------------------------\n\n");

    // Allocate memory for the total number of freespace blocks
    uint8_t * freespaceLBA = malloc((sizeof(uint8_t) * aVCB_ptr->numberOfBlocks));

    // Get the updated freespace from LBA and set it equal to 'freespaceLBA'.
    // Now we have an updated version in memory
    LBAread(freespaceLBA, aVCB_ptr->freeSpaceBlocks, aVCB_ptr->LBA_indexOf_freeSpace);

    // Iterate over the total number of blocks specified we'd like to allocate
    int i;
    for(i = 0; i < numBlocksToAlloc; i++) {

        // Set the specified blocks to 1, meaning they are now allocated
        freespaceLBA[blockPos + i] = 1;
    }

    // Now write the updated freespace back to the LBA
    uint64_t blocksWritten = LBAwrite(freespaceLBA, aVCB_ptr->freeSpaceBlocks, aVCB_ptr->LBA_indexOf_freeSpace);

    // Free the memory allocated for total number of freespace blocks
    free(freespaceLBA);

    // Return the number of blocks written to LBA
    return blocksWritten;
}

// Deallocate some amount of blocks from the LBA
uint64_t deallocFSBlocks(VCB * aVCB_ptr, uint64_t numBlocksToDealloc, uint64_t blockPos) {

    // Allocate memory for the total number of freespace blocks
    uint8_t * freespaceLBA = malloc((sizeof(uint8_t) * aVCB_ptr->numberOfBlocks));

    // Get the updated freespace from LBA and set it equal to 'freespaceLBA'
    // so we have an updated version saved in memory
    LBAread(freespaceLBA, aVCB_ptr->freeSpaceBlocks, aVCB_ptr->LBA_indexOf_freeSpace);

    // Iterate over the total number of blocks specified we'd like to deallocate
    int i;
    for(i = 0; i < numBlocksToDealloc; i++) {

        // Set the specified blocks to 0, meaning they are now deallocated
        freespaceLBA[blockPos + i] = 0;
    }

    // Now write the updated freespace back to the LBA
    uint64_t blocksWritten = LBAwrite(freespaceLBA, numBlocksToDealloc, blockPos);

    // Free the memory allocated for total number of freespace blocks
    free(freespaceLBA);

    // Return the number of blocks written to LBA
    return blocksWritten;
}

// Request a certain amount of blocks from the LBA
uint64_t requestFSBlocks(VCB * aVCB_ptr, uint64_t numFSBlocksRequested) {

    // Allocate memory for the total number of freespace blocks
    uint8_t * freespaceLBA = malloc((sizeof(uint8_t) * aVCB_ptr->numberOfBlocks));

    // Get the updated freespace from LBA and set it equal to 'freespaceLBA'
    // so we have an updated version saved in memory
    LBAread(freespaceLBA, aVCB_ptr->freeSpaceBlocks, aVCB_ptr->LBA_indexOf_freeSpace);

    // Iterate over the total number of blocks on the LBA
    int i;
    for(i = 0; i < aVCB_ptr->numberOfBlocks; i++) {

        // Now iterate through the freespace to determine where the index of the next available freespace is
        int j;
        for(j = i; freespaceLBA[j] == 0 && j < aVCB_ptr->numberOfBlocks; j++) {

            // if 'j - i' equals the amount of blocks requested, then return the index
            if(j - i >= numFSBlocksRequested) {

                free(freespaceLBA);
                return i;
            }
        }
    }

    // Free the memory allocated for total number of freespace blocks
    free(freespaceLBA);

    return 0;
}