/**************************************************************
* Class:  CSC-415
* Names: Tania Nemeth, Battulga Tsogtgerel, Mitchel Baker, Hanwen Geng
* Student IDs: 920680290, 920203983, 917679066, 920623935 (respectively to name order)
* Project: Basic File System
*
* File: mfs.c
*
* Description: Below are the complete implementations of the mfc.h functions.
*              The functions below are called when a user inputs a command
*              into the shell prompt.
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

// Global variables representing the current working directory and root directories
ino_t curDir = 0;
ino_t rootDir = 0;

// Global variable for block size
u_int64_t blocksizeCache;

// Get total number of bytes in terms of block size
u_int64_t getBytes(uint64_t bytes) {
    return ((bytes / blocksizeCache) + 1) * blocksizeCache;
}

// Initialize temporary VCB attributes for use in the rest of the functions (may not be necessary)
int temp_init() {

    // Get updated VCB from LBA
    struct VCB * vcb = malloc(MINBLOCKSIZE);
    getVCB(vcb);

    // Initialize the current working directory to the root directory
    curDir = vcb->LBA_indexOf_rootDir;

    // Set it equal to the size of block
    blocksizeCache = vcb->sizeOfBlock;

    free(vcb);
    return 0;
}

// Setup and initialize the file system
int fs_init(uint64_t volumeSize, uint64_t blockSize) {
    
    // Return value, -1 changed to 0 on success
    int fs_init_success = -1;
   
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

        // Set current working directory equal to the LBA index of the root directory
        // By default, the cwd will be set to root since the shell opens up at the root directory
        curDir = aVCB_ptr->LBA_indexOf_rootDir;
	}

    free (aVCB_ptr);
    free (root);

    aVCB_ptr = NULL;
    root = NULL;

    return fs_init_success;
}

//function that returns d_entry * and takes a path
//need to add stuff bc rn can only handle if theres no "/" (relative) 
//if '/' starts from root (absolute)
int get_entry_from_path(struct VCB *vcb, char * path, d_entry * entry){
    
    //calculating num of blocks to read and allocating space 
    uint64_t dirNumBlocks = (sizeof(Directory) / vcb->sizeOfBlock) + 1;
    Directory *cd = malloc(dirNumBlocks * vcb->sizeOfBlock);
    //reading the directory out

    d_entry *cur;
    // absolute path
    if(path[0] == '/'){
        LBAread(cd, dirNumBlocks, vcb->LBA_indexOf_rootDir); 
        cur = &cd->entries[0];
    }

    //relative path
    else {
        LBAread(cd, dirNumBlocks, curDir); 
        cur = &cd->entries[0];
    }

    //copy path since strtok changes things
    char *path_copy = malloc(strlen(path));
    strcpy(path_copy, path);
    char *next_dir = strtok(path_copy, "/");
    while (next_dir != NULL) {
        LBAread(cd, dirNumBlocks, cur->d_ino);
        bool found = false;
        for(int i =0; i < cd->size; i++){
            if(strcmp(cd->entries[i].d_name, next_dir) == 0){
                //if equal has been located so set cur to the next dir entry
                cur = &cd->entries[i];
                found = true;
                break;
            }
        }
        if (!found) {
            // couldn't find next directory
            free(cd);
            free(path_copy);

            cd = NULL;
            path_copy = NULL;

            return 1;
        }

        next_dir = strtok(NULL, "/");
    }

    // we found it so copy into entry and return success
    memcpy(entry, cur, sizeof(d_entry));

    free(cd);
    free(path_copy);

    cd = NULL;
    path_copy = NULL;

    return 0;
}

// Get the current working directory (pwd)
char * fs_getcwd(char *buf, size_t size) {

    struct VCB * myVCB = malloc(512);
    myVCB = getVCB(myVCB);

    uint64_t numDirBlocks = (sizeof(Directory) / myVCB->sizeOfBlock) + 1;

    Directory * curr = malloc(numDirBlocks * myVCB->sizeOfBlock);
    
    // Start at current directory
    LBAread(curr, numDirBlocks, curDir);

    // DEBUG prints
    //printf("current dir name: %s\n", curr->name);
    //printf("current dir parent: %ld\n", curr->parent);

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

        directoryNames[i] = NULL;
    }
    free(directoryNames);
    free(myVCB);
    free(curr);

    directoryNames = NULL;
    myVCB = NULL;
    curr = NULL;

    return fullPath;
}

// Set the current working directory in the file system (cd) 
int fs_setcwd(char *buf) {
    struct VCB *vcb = malloc(512);
    vcb = getVCB(vcb);

    printf("path=%s cwd=%ld\n", buf, curDir);
    d_entry entry;
    if(get_entry_from_path(vcb, buf, &entry)==0){
        printf("entry ino: %ld\n", entry.d_ino);
        //success so directory entry for given path has been found
        //set the cwd var to innode 
        curDir = entry.d_ino;

        free(vcb);

        vcb = NULL;

        return 0;
    }

    free(vcb);

    vcb = NULL;

    return 1;
}

// Removes a directory
int fs_rmdir(const char * pathname) {

    printf("pathname: %s, cwd: %ld\n", pathname, curDir);

    // Get updated VCB from LBA
    struct VCB * vcb = malloc(MINBLOCKSIZE);
    getVCB(vcb);

    // Initialize to number of blocks for a directory
    uint64_t dirNumBlocks = (sizeof(fdDir) / vcb->sizeOfBlock) + 1;

    // inode position in LBA for the directory to remove
    ino_t rmDirPos;

    // inode position in a directory's 'entries' parameters
    ino_t rmDirEntryPos;

    // Allocate memory for parent dir
    Directory * dir = malloc(dirNumBlocks * vcb->sizeOfBlock);

    // Only set to true if we have found the directory entry we want to remove
    bool found = false;

    // Read into 'dir' the current working directory
    LBAread(dir, dirNumBlocks, vcb->LBA_indexOf_rootDir);

    // Begin tokenization to find the directory entry to remove
    char * token;
    size_t pathSize = strlen(pathname);
    char * name = malloc(pathSize + 1);
    memcpy(name, pathname, pathSize + 1);
    token = strtok(name, "/");
    int counter = 0;

    while(counter < dir->size && token != NULL) {
        
        // '0' indicates we have found the directory entry which matches the 'pathname' specified
        if(strcmp(token, dir->entries[counter].d_name) == 0) {

            // DEBUG for seeing if token and d_name match
            //printf("TOKEN: %s\n", token);
            //printf("dir->entries[counter].d_name: %s\n", dir->entries[counter].d_name);

            token = strtok(NULL, "/");

            if(token == NULL) {
                
                found = true;
                rmDirEntryPos = counter;
                rmDirPos = dir->entries[counter].d_ino;
            }
        }
        else {
            counter++;
        }
    }

    // If the entry specified from 'pathname' is found, then deallocate the entry's freespace allocation,
    // remove the entry from its parent, and write the updated parent Directory to LBA
    if(found) {

        // Deallocate the size of the Directory to remove from freespace
        deallocFSBlocks(vcb, ((sizeof(Directory) / MINBLOCKSIZE) + 1), rmDirPos);

        // Now remove the directory entry from its parent
        for(int i = rmDirEntryPos; i < dir->size; i++) {
            dir->entries[i] = dir->entries[i + 1];
        }

        // Now decrement the size of parent Directory
        dir->size--;

        // Update the directory in LBA
        LBAwrite(dir, ((sizeof(* dir) / MINBLOCKSIZE) + 1), vcb->LBA_indexOf_rootDir);
    }

    free(name);
    free(dir);
    free(vcb);

    name = NULL;
    dir = NULL;
    vcb = NULL;

    return 0;
}

//open directory
fdDir * fs_opendir(const char *name){
    struct VCB *vcb = malloc(512);
    getVCB(vcb);

    d_entry entry;
    get_entry_from_path(vcb, name, &entry);

    fdDir * ret = malloc(sizeof(fdDir));
    ret->d_reclen = sizeof(fdDir);

    uint64_t dirNumBlocks = (sizeof(Directory) / vcb->sizeOfBlock) + 1;

    Directory *dir = malloc(dirNumBlocks * vcb->sizeOfBlock);
    LBAread(dir, dirNumBlocks, entry.d_ino);

    ret->numEntries = dir->size;
    ret->entries = malloc(sizeof(struct fs_diriteminfo) * dir->size);
    ret->curEntry = 0;

    for (int i = 0; i < dir->size; i++) {
        strcpy(ret->entries[i].d_name, dir->entries[i].d_name);
        ret->entries[i].d_ino = dir->entries[i].d_ino;
        ret->entries[i].fileType = dir->entries[i].d_type;
        ret->entries[i].d_reclen = sizeof(struct fs_diriteminfo);
    }

    free(dir);

    dir = NULL;

    return ret;
}

 // Call this function after rmdir()
int fs_closedir(fdDir *dirp){

    // Free memory allocated for each directory entry
    free(dirp->entries);

    // Free the directory itself
    free(dirp);

    dirp->entries = NULL;
    dirp = NULL;

    return 0;
}

struct fs_diriteminfo *fs_readdir(fdDir *dirp){
    if (dirp->curEntry >= dirp->numEntries) {
        return NULL;
    }
    return &dirp->entries[dirp->curEntry++];
}

int fs_stat(const char *path, struct fs_stat *buf) {

    struct stat * path_stat;
    path_stat = (struct stat *) buf;

    return(stat(path, path_stat));
}

//return 1 if file, 0 otherwise
int fs_isFile(char * path){
    //find item referenced by path:
    //start at root directory which has well known inode
    //find next link in path 
    //find inode from file once located (directory entry has innode)
    struct VCB *vcb = malloc(512);
    getVCB(vcb);
    d_entry entry;
      if(get_entry_from_path(vcb, path, &entry)==0){
        //success so directory entry for given path has been found
        if(entry.d_type == 'f'){
            printf("file found\n");
            
            free(vcb);

            vcb = NULL;

            return 1;
        }
        free(vcb);

        vcb = NULL;

        return 0;
    }
    printf("item by this name does not exist in the file system\n");
    free(vcb);

    vcb = NULL;

    return 0;
}	

int fs_isDir(char * path) {		//return 1 if directory, 0 otherwise

  //find item referenced by path:
    //start at root directory which has well known inode
    //find next link in path 
    //find inode from file once located (directory entry has innode)
    struct VCB *vcb = malloc(512);
    getVCB(vcb);
    d_entry entry;
        if(get_entry_from_path(vcb, path, &entry)==0){
        //success so directory entry for given path has been found
            if(entry.d_type == 'd'){
                printf("directory found\n");
                free(vcb);

                vcb = NULL;

                return 1;

        }
        free(vcb);

        vcb = NULL;

        return 0;
    }
    printf("directory by this name does not exist\n");
    free(vcb);
    return 0;
}

//removes a file
int fs_delete(char* filename){
    //return -1;
    //Return: -1 if there is no such file to delete(if file is opened)
    //Return 0 on success
    struct VCB *vcb = malloc(512);
    getVCB(vcb);
    d_entry entry;
    get_entry_from_path(vcb, filename, &entry);

    uint64_t dirNumBlocks = (sizeof(Directory) / vcb->sizeOfBlock) + 1;
    Directory *dir = malloc(dirNumBlocks * vcb->sizeOfBlock);
    LBAread(dir, dirNumBlocks, entry.parent);

    ino_t rmDirEntryPos;
    //d_entry * rmdirEntry = malloc(sizeof(* rmdirEntry));
    bool found = false;

    char * token;
    char * name = malloc(sizeof(char) * strlen(filename));
    strcpy(name, filename);
    token = strtok(name, "/");
    int counter = 0;

    while(counter < dir->size && token != NULL) {
        if(strcmp(token, dir->entries[counter].d_name) == 0) {

            printf("TOKEN: %s\n", token);
            printf("dir->entries[counter].d_name: %s\n", dir->entries[counter].d_name);

            token = strtok(NULL, "/");

            if(token == NULL) {
        
                found = true;
                rmDirEntryPos = counter;
                ino_t rmDirPos = dir->entries[counter].d_ino;
                
                //uint64_t dirEntryNumBlocks = (sizeof(dir->entries[counter]) / vcb->sizeOfBlock) + 1;
                //LBAread(rmdirEntry, dirEntryNumBlocks, rmDirPos);
            }
        }
        else {
            counter++;
        }
    }

    if(found) {

        // Deallocate the size of the File to remove from freespace
        deallocFSBlocks(vcb, ((sizeof(d_entry) / MINBLOCKSIZE) + 1), rmDirEntryPos);

        // Now remove the entry from its parent
        for(int i = rmDirEntryPos; i < dir->size; i++) {
            dir->entries[i] = dir->entries[i + 1];
        }

        // Now decrement the size of parent Directory
        dir->size--;

        LBAwrite(dir, ((sizeof(* dir) / MINBLOCKSIZE) + 1), entry.parent);
    }

    free(name);
    free(dir);
    free(vcb);

    name = NULL;
    dir = NULL;
    vcb = NULL;

    return 0;
}

// Make a new directory 
int fs_mkdir(const char *pathname, mode_t mode) {

    printf("pathname: %s\n", pathname);

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
        LBAread(curr, (sizeof(*curr) / myVCB->sizeOfBlock) + 1, curDir);
    }

    /* Allocating blocks for new directory */
    uint64_t newDirBlocks = (sizeof(* curr) / myVCB->sizeOfBlock) + 1;

    uint64_t newDirPosition = requestFSBlocks(myVCB, newDirBlocks);

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
    free(myVCB);

    curr = NULL;
    myVCB = NULL;

    return 1;
}

// Create a new path when we move a directory to a new position
int fs_createNewPath(char * newPath, struct d_entry oldDirEntry) {

    // Get updated VCB from LBA
    struct VCB * vcb = malloc(MINBLOCKSIZE);
    getVCB(vcb);

    // Initialize to number of blocks for a directory
    uint64_t dirNumBlocks = (sizeof(Directory) / vcb->sizeOfBlock) + 1;

    // Allocate memory for parent dir
    Directory * dir = malloc(dirNumBlocks * vcb->sizeOfBlock);

    // Read into 'dir' the current working directory
    LBAread(dir, dirNumBlocks, vcb->LBA_indexOf_rootDir);

    uint64_t newDirBlocks = (sizeof(Directory) / vcb->sizeOfBlock) + 1;

    // Allocate memory for the new directory 
    Directory * newDir = malloc(newDirBlocks * vcb->sizeOfBlock);

    // We already know what the old Directory is.
    // Now, use strtok() to find the Directory to copy the other directory into 
    char * token;
    token = strtok(newPath, "/");
    int counter = 0;
    char new_d_name[MAX_NAME_LEN];

    while(counter < dir->size && token != NULL) {

        // Skip over '.' since this is not what we are looking for
        if(strcmp(token, ".") == 0) {
            
            token = strtok(NULL, "/");
        }
        // If token matches the entry's d_name, then we have found the directory to move the 'oldDirEntry' into
        else if(strcmp(token, dir->entries[counter].d_name) == 0) {

            //printf("token: %s\n", token);
            //printf("d_name: %s\n", dir->entries[counter].d_name);
            //printf("d_ino: %ld\n", dir->entries[counter].d_ino);

            // Now that we have the directory to move 'oldDirEntry' into, create a new Directory for it
            Directory * newDirectory = malloc(newDirBlocks * vcb->sizeOfBlock);

            // Read into the position of the directory to copy into 
            LBAread(newDirectory, newDirBlocks, dir->entries[counter].d_ino);

            // Set its name and parent
            strcpy(newDirectory->name, dir->entries[counter].d_name);
            newDirectory->parent = curDir;

            // Now, initialize the entries in our new directory
            for(int idx = 0; idx < MAX_NUM_ENTRIES; idx++) {

                // First available entry position in 'entries'
                // Add the 'oldDirEntry' into the 0th position
                if(idx == 0) {

                    // Initialize '.' and its attributes
                    strcpy(newDirectory->entries[0].d_name, ".");
                    newDirectory->entries[0].d_free = false;
                    newDirectory->entries[0].d_type = 'd';
                    newDirectory->entries[0].d_ino = dir->entries[counter].d_ino;

                    // Initialize '..' and its attributes 
                    strcpy(newDirectory->entries[1].d_name, "..");
                    newDirectory->entries[1].d_free = false;
                    newDirectory->entries[1].d_type = 'd';
                    newDirectory->entries[1].d_ino = curDir;

                    // Initialize the old Directory entry into the Directory we are moving it into 
                    strcpy(newDirectory->entries[2].d_name, oldDirEntry.d_name);
                    newDirectory->entries[2].d_ino = oldDirEntry.d_ino;
                    newDirectory->entries[2].parent = dir->entries[counter].d_ino;
                    newDirectory->entries[2].d_type = oldDirEntry.d_type;
                    newDirectory->entries[2].d_len = oldDirEntry.d_len;
                    newDirectory->entries[2].d_free = oldDirEntry.d_free;

                    // Now read the old directory entry from its inode position
                    LBAread(newDir, newDirBlocks, oldDirEntry.d_ino);

                    // Now initialize the new Directory's name, size, and parent
                    strcpy(newDir->name, oldDirEntry.d_name);
                    newDir->size = 2;

                    // Parent will be set to the inode of the Directory it is being moved into 
                    newDir->parent = dir->entries[counter].d_ino;

                    // Iterate over the entries of the new directory, initialize its parent
                    // and set each entry to free
                    for(int i = 0; i < MAX_NUM_ENTRIES; i++) {
                        newDir->entries[i].parent = oldDirEntry.d_ino;
                        newDir->entries[i].d_free = true;
                    }

                    // Initialize '.' and its attributes for the new Directory
                    strcpy(newDir->entries[0].d_name, ".");
                    newDir->entries[0].d_free = false;
                    newDir->entries[0].d_type = 'd';
                    newDir->entries[0].d_ino = oldDirEntry.d_ino;
                    
                    // Initialize '..' and its attributes for the new Directory 
                    strcpy(newDirectory->entries[1].d_name, "..");
                    newDirectory->entries[1].d_free = false;
                    newDirectory->entries[1].d_type = 'd';
                    newDirectory->entries[1].d_ino = dir->entries[counter].d_ino;

                    // Now write the old Directory into its inode position so that it is 
                    // updated in the LBA
                    LBAwrite(newDir, newDirBlocks, oldDirEntry.d_ino);

                    newDirectory->size = 3;
                    idx = 2;
                }

                newDirectory->entries[idx].parent = dir->entries[counter].d_ino;
                newDirectory->entries[idx].d_free = true;
            }

            // Lastly, write the Directory being moved into and update it into the LBA
            LBAwrite(newDirectory, newDirBlocks, newDir->parent);

            free(newDirectory);

            newDirectory = NULL;

            token = strtok(NULL, "/");
        }
        else {
            counter++;
        }
    }

    free(dir);
    free(newDir);
    free(vcb);

    dir = NULL;
    newDir = NULL;
    vcb = NULL;

    return 0;
}

// Remove the old Directory from its original position. This is necessary when we 
// move a Directory into a new directory position
int fs_removeOldEntry(Directory * dir, ino_t entry_ino, int counter) {

    // Get updated VCB from LBA
    struct VCB * vcb = malloc(MINBLOCKSIZE);
    getVCB(vcb);

    // Now remove the directory entry from its parent
    for(int i = counter; i < dir->size; i++) {
        dir->entries[i] = dir->entries[i + 1];
    }
    // Now decrement the size of parent Directory
    dir->size--;

    // Update the directory into the LBA
    LBAwrite(dir, ((sizeof(* dir) / MINBLOCKSIZE) + 1), vcb->LBA_indexOf_rootDir);

    return 0;
}

// Move a Directory from its original position into a new position
int fs_mv(char * oldPath, char * newPath) {

    // Get updated VCB from LBA
    struct VCB * vcb = malloc(MINBLOCKSIZE);
    getVCB(vcb);

    // Initialize to number of blocks for a directory
    uint64_t dirNumBlocks = (sizeof(Directory) / vcb->sizeOfBlock) + 1;

    // Allocate memory for parent dir
    Directory * dir = malloc(dirNumBlocks * vcb->sizeOfBlock);

    // Read into 'dir' the current working directory
    LBAread(dir, dirNumBlocks, vcb->LBA_indexOf_rootDir);

    // Now tokenize the directory entries in order to determine which entry will be moved
    char * token;
    token = strtok(oldPath, "/");
    int counter = 0;

    while(counter < dir->size && token != NULL) {

        if(strcmp(token, dir->entries[counter].d_name) == 0) {

            //printf("token: %s\n", token);
            //printf("d_name: %s\n", dir->entries[counter].d_name);

            token = strtok(NULL, "/");

            // We have found the entry if the token is set equal to NULL
            if(token == NULL) {
                // Create the new path for the Directory entry to be moved 
                fs_createNewPath(newPath, dir->entries[counter]);

                // Lastly, remove the directory from its original position
                fs_removeOldEntry(dir, dir->entries[counter].d_ino, counter);
            }
        }
        else {
            counter++;
        }
    }

    free(dir);
    free(vcb);

    dir = NULL;
    vcb = NULL;

    return 0;
}

// Close the file system
int fs_close(){
    closePartitionSystem();
}