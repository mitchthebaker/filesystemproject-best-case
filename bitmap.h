/**************************************************************
* Class:  CSC-415
* Name: Mitchel Baker and Tania Nemeth
* Student ID: 917679066
* Project: Basic File System
*
* File: bitmap.h
*
* Description: The interface for bitmap.c. The declarations for
*              all bitmap functions are declared here.
*
**************************************************************/

#include <limits.h>
#include "fsVCB.h"

#ifndef BITMAP_H
#define BITMAP_H

// Initialize the bitmap for freespace.
// Pass in a pointer to the VCB, which is used to calculate the total number of freespace blocks
int map_initialize(VCB * aVCB_ptr);

// Pass in a pointer to the VCB, the total number of new blocks to allocate, and the block 
// position to allocate the new blocks at
uint64_t allocFSBlocks(VCB * aVCB_ptr, uint64_t numBlocksToAlloc, uint64_t blockPos);

// Pass in a pointer to the VCB, the total number of existing blocks to deallocate, and the 
// block position to deallocate the existing blocks from
uint64_t deallocFSBlocks(VCB * aVCB_ptr, uint64_t numBlocksToDealloc, uint64_t blockPos);

// Pass in a pointer to the VCB, in addition to the total number of new blocks to request from the freespace bitmap
uint64_t requestFSBlocks(VCB * aVCB_ptr, uint64_t numFSBlocksRequested);

#endif