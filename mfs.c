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
#include "fsDir.h"

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
        int init_s = initVCB(aVCB_ptr, volumeSize, blockSize);
        int load_s = loadVCB(aVCB_ptr);

        // Check return values and update fs_init() return var
        fs_init_success = (!init_s) ? 0 : fs_init_success; 
		fs_init_success = (!load_s) ? 0 : fs_init_success;

        // Allocate memory for bitmap, then write to LBA
        //map_init(aVCB_ptr, aBitmap, sizeOfBitmap, totalNumBytes);
        int freeSpaceBlocksWritten = map_initialize(aVCB_ptr);
        
		//initialize directory here?
		//something like new directory = initDirectory(parent) --> should be null for root
		uint64_t initRoot_s = initRootDir(aVCB_ptr, root, freeSpaceBlocksWritten);

        curDir = aVCB_ptr->LBA_indexOf_rootDir;
	}

    free (aVCB_ptr);
    free (root);

    aVCB_ptr = NULL;
    root = NULL;

    return fs_init_success;
}

//function that returns d_entry * and takes a path
int get_entry_from_path(char * path, d_entry * entry){
    Directory cd;
    LBAread(&cd, 1, curDir); //represents current directory
    //look through entries associated with cd 
    for(int i =0; i < cd.size; i++){
        printf("path option: %s\n", cd.entries[i].d_name);
        if(strcmp(cd.entries[i].d_name, path) == 0){
            //if equal has been located so memcpy into entry 
            memcpy(entry, &cd.entries[i], sizeof(d_entry));
            //success
            return 0;

        }
    

    }
    //not found
    return 1;
}

char * fs_getcwd(char *buf, size_t size) {
    //easier to put cwd in VCB 

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

    //should start at current directory instead of root 
    LBAread(curr, 1, curDir);
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
  
    printf("path=%s cwd=%ld\n", buf, curDir);
    d_entry entry;
    if(get_entry_from_path(buf, &entry)==0){
        printf("entry ino: %ld\n", entry.d_ino);
        //success so directory entry for given path has been found
        //set the cwd var to innode 
        curDir = entry.d_ino;
        return 0;
    }

    return 1;
    
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

//open directory
fdDir * fs_opendir(const char *name){
    d_entry entry;
    get_entry_from_path(name, &entry);
    
    return 0;
}

//return 1 if file, 0 otherwise
int fs_isFile(char * path){
    //find item referenced by path:
    //start at root directory which has well known inode
    //find next link in path 
    //find inode from file once located (directory entry has innode)


}	

//removes a file
int fs_delete(char* filename){
    //find file (fs open)

}	


int fs_close(){
    closePartitionSystem();
}



int fs_mkdir(const char *pathname, mode_t mode) {

    struct VCB *myVCB = malloc(512);
    getVCB(myVCB);

    Directory *curr = malloc(getBytes(sizeof(*curr)));

    /* Reading current directory and its entries */
    if (pathname[0] == '/') {
        // do we know rootDir is always 1 block?
        // might have to change this 1 
        LBAread(curr, 1, myVCB->LBA_indexOf_rootDir);
    } else {
        //curr = malloc(getBytes(sizeof(*curr)));
        printf("%ld\n", myVCB->sizeOfBlock);
        LBAread(curr, (sizeof(*curr) / myVCB->sizeOfBlock) + 1, curDir);

    }

    /* Allocating blocks for new directory */
    uint64_t newDirBlocks = (sizeof(* curr) / myVCB->sizeOfBlock) + 1;
    printf("size of curr: %ld\n", sizeof(* curr));
    printf("size of curr / blockSize: %ld\n", (sizeof(* curr) / myVCB->sizeOfBlock + 1));
    printf("newDirBlocks: %ld\n", newDirBlocks);

    uint64_t newDirPosition = requestFSBlocks(myVCB, newDirBlocks);
    printf("newDirPosition: %ld\n", newDirPosition);

    allocFSBlocks(myVCB, newDirBlocks, newDirPosition);

    /* Initializing new Directory and saving */
    Directory *newDir = malloc(getBytes(sizeof(*newDir)));
    newDir->size = 2;
    newDir->parent = curDir;
    strcpy(newDir->name, pathname);
    for(int i = 0; i < MAX_NUM_ENTRIES; i++) {
        newDir->entries[i].parent = curDir;
        newDir->entries[i].d_free = true;
    }

    strcpy(newDir->entries[0].d_name, ".");
    newDir->entries[0].d_free = false;
    newDir->entries[0].d_type = 'd';
    newDir->entries[0].d_ino = newDirPosition;

    strcpy(newDir->entries[1].d_name, "..");
    newDir->entries[1].d_free = false;
    newDir->entries[1].d_type = 'd';
    newDir->entries[1].d_ino = newDir->parent;

    LBAwrite(newDir, newDirBlocks, newDirPosition);
    free(newDir);

    /* Making new entry for the parent */
    uint64_t sizeInBlocks = (sizeof(struct Directory) * curr->size);
    curr->entries[curr->size].d_ino = newDirPosition;
    curr->entries[curr->size].d_free = false;
    curr->entries[curr->size].d_type = 'd';
    //works for now but should be filename instead of pathname later 
    strcpy(curr->entries[curr->size].d_name, pathname);
    curr->size++;

    uint64_t newSizeInBlocks = (sizeof(struct Directory) * curr->size) / myVCB->sizeOfBlock + 1;
    LBAwrite(curr, (sizeof(*curr) / myVCB->sizeOfBlock) + 1, curDir);

    free(curr);

    return 1;
}