#include <limits.h>
#include "fsVCB.h"

#ifndef BITMAP_H
#define BITMAP_H

int map_initialize(VCB * aVCB_ptr);

uint64_t allocFSBlocks(VCB * aVCB_ptr, uint64_t numBlocksToAlloc, uint64_t blockPos);

uint64_t deallocFSBlocks(VCB * aVCB_ptr, uint64_t numBlocksToDealloc, uint64_t blockPos);

uint64_t requestFSBlocks(VCB * aVCB_ptr, uint64_t numFSBlocksRequested);

#endif