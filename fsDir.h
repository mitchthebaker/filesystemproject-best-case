/**************************************************************
* Class:  CSC-415
* Name: Tania Nemeth
* Student ID: 920680290
* Project: Basic File System
*
* File: fsDir.h
*
* Description: This is the header file for fsVCB.c; the functions 
*              declared below are necessary for loading and 
*              initializing our directory system and roote directory in the 
*              first block of the LBA.
*
**************************************************************/
#include "fsLow.h"
#include <sys/stat.h>

//do i need this? Directory structure --> root directory to initialize?
typedef struct Directory {
    char name;
    int size;
    d_entry entries[];      //should initialize this way (dynamically allocate) d_entry * entries or with size in it

} Directory;


//directory should contain array of entries ?
typedef struct d_entry {
    ino_t d_ino;                // file serial #
    char d_name;                // name of entry 
    char d_name_len;            // length of entry name
    char d_type;               // type of entry
    uint16_t d_len;           // length of entry 
} d_entry;

