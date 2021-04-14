/**************************************************************
* Class:  CSC-415
* Name: Tania Nemeth
* Student ID: 920680290
* Project: Basic File System
*
* File: fsDir.h
*
* Description: This is the header file for fsVCB.c; the structures 
*              declared below are necessary for
*              initializing our directory system and the root directory
*
**************************************************************/
#include "fsLow.h"
#include <sys/stat.h>
#include <stdbool.h>
#define MAX_NUM_ENTRIES 10
#define MAX_NAME_LEN 30

//directory should contain array of entries 
typedef struct d_entry {
    ino_t d_ino;                // file serial #
    ino_t parent;               
    char d_name[MAX_NAME_LEN];  // name of entry 
    char d_type;               // type of entry (d = directory, f = file)
    uint16_t d_len;           // length of entry 
    bool d_free;               //indicates whether DE is free 
} d_entry;

//Directory structure --> root directory to initialize
typedef struct Directory {
    char name;
    int size;
    d_entry *entries;      //should initialize this way (dynamically allocate) d_entry * entries

} Directory;




