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
#include "mfs.h"
#include "fsLow.h"
#include "fsVCB.h"
//setup 
int fs_init(){
    //file writing into, block size, etc.,
    startPartitionSystem("file_system.hex", 4096, 128);
   
    //Initialize our VCB struct, allocate enough memory to account for all its parameters
	VCB * aVCB_ptr = malloc(sizeof(* aVCB_ptr));
    initVCB(aVCB_ptr, 4096, 128);
    //load into file system
    loadVCB(aVCB_ptr);
    //write root directory into the file system
    initRootDir(aVCB_ptr);


}


int fs_close(){
    closePartitionSystem();
}