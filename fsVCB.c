/**************************************************************
* Class:  CSC-415
* Name: Mitchel Baker and Tania Nemeth
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
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>

// Initialize our VCB with its default parameters.
//
// Expand upon the VCB and add any new parameters in 'fsVCB.h' if necessary 
int initVCB(VCB * aVCB_ptr, uint64_t volumeSize, uint64_t blockSize) {

    char title[] = "VCB Header - Best Case";
    strcpy(aVCB_ptr->vcbTitle, title);
    aVCB_ptr->numberOfBlocks = volumeSize / blockSize;
    aVCB_ptr->sizeOfBlock = blockSize;
    aVCB_ptr->magicNumber = MAGIC_NUMBER;

    // Test to see if VCB is initialized with default params
    printf("\ntitle: %s\n", aVCB_ptr->vcbTitle);
	printf("numBlocks: %ld\n", aVCB_ptr->numberOfBlocks);
    printf("sizeOfBlocks: %ld\n", aVCB_ptr->sizeOfBlock);
    printf("magic num: %d\n", aVCB_ptr->magicNumber);

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
//Tania 
d_entry * createAndInitDir (ino_t parent, VCB * aVCB_ptr){
    //function that will create and initizalize empty directory entries in memory 
    d_entry * dir;     //allocate space for the directory 
     //need to get the # of directory entries and size of DE to figure out # of bytes to allocate
    int size_dir_entries = (MAX_NUM_ENTRIES * sizeof(d_entry)); 
     //multiply and add (block size - 1) then / by block size to get the # of blocks 
    int num_blocks = (size_dir_entries + (aVCB_ptr->sizeOfBlock -1)) / aVCB_ptr->sizeOfBlock;
    //populate with initizalied, default, empty directory entries (malloc and init array of DE's to default empty entries)
    dir = malloc(num_blocks * aVCB_ptr->sizeOfBlock);
    //need to indicate that the entries are free and set the parent
    for(int i = 0; i < MAX_NUM_ENTRIES; i++){
        dir[i].parent = parent;
        dir[i].d_free = true;
    }
    return dir;

}
int initRootDir(VCB * aVCB_ptr, uint64_t sizeOfBitmap){
    d_entry * root = createAndInitDir(0, aVCB_ptr);
    //point lba to where the freespace ends, (index of freespace = 1 + sizeOfBitmap)
    int LBA = aVCB_ptr -> LBA_indexOf_freeSpace + sizeOfBitmap;
    printf("LBA: %d\n", LBA);
    int size_dir_entries = (MAX_NUM_ENTRIES * sizeof(d_entry)); 
     //multiply and add (block size - 1) then / by block size to get the # of blocks 
    int num_blocks = (size_dir_entries + (aVCB_ptr->sizeOfBlock -1)) / aVCB_ptr->sizeOfBlock;
    //write root directory to LBA
    LBAwrite(root, num_blocks, LBA);
    //LBAwrite(root, num_blocks, 1);

    //location is LBA
    //size is # of dir entries * size of directory entry

    //create directory entry for .
    strcpy(root[0].d_name, ".");
    root[0].d_free = false;
    //type of file is directory
    root[0].d_type = 'd';

    //for .. if parent == null it's the LBA, otherwise: its the parent
    //if parent == null ? LBA : parent
    //size = parent (sizeof(parent)), type of file is directory
    //create directory entry for ..
    strcpy(root[1].d_name, "..");
    root[1].d_free = false;
    //type of file is directory
    root[1].d_type = 'd';

    //write directory entries to the file system
    LBAwrite(root, num_blocks, LBA);
    aVCB_ptr->LBA_indexOf_rootDir = LBA;
    root->parent = LBA;
    aVCB_ptr->LBA_indexOf_freeSpace = LBA + num_blocks;
    //https://www.thegeekstuff.com/2012/06/c-directory/
    // fs_mkdir("/", 0777);
    


}

//end Tania

// Load our VCB into the LBA at logical block 0
int loadVCB(VCB * aVCB_ptr) {

    // Allocate 512 bytes and read block 0 from LBA
    char * aBuffer = malloc(MINBLOCKSIZE);

    // Write our VCB struct to logical block 0
    // uint64_t blocksWritten = LBAwrite(aVCB_ptr, 1, 0);
    // printf("\nWriting to LBA...\n");
    // printf("blocksWritten: %ld\n", blocksWritten);

    // Lets check if our VCB was written to the LBA
    LBAread(aBuffer, 1, 0);

    printf("Block 0 of volume: %s\n\n", aBuffer);

    // Cleanup
    free(aBuffer);

    // 'blocksWritten' will be 1 if the write was successful, therefore return 0 (OK)
    // -1 will be returned if the write to disk failed
    return 0; // @TODO FIX THIS LATER 
    // return (blocksWritten != 1) ? -1 : 0;
}