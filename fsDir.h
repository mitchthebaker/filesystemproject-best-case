/**************************************************************
* Class:  CSC-415
* Name: Tania Nemeth
* Student ID: 920680290
* Project: Basic File System
*
* File: fsDir.h
*
* Description: This is the header file for fsVCB.c; the functions 
*              declared below are necessary for
*              initializing our directory system and the root directory
*
**************************************************************/
#include "fsLow.h"
#include <stdint.h>
#include <sys/stat.h>
#define MAX_NUM_ENTRIES 10
#define MAX_NAME_LEN 30

//directory should contain array of entries 
typedef struct d_entry {
    ino_t d_ino;                // file serial #
    char d_name[MAX_NAME_LEN];  // name of entry 
    //char d_name_len;            // length of entry name
    char d_type;               // type of entry
    uint16_t d_len;           // length of entry 
} d_entry;

//Directory structure --> root directory to initialize
typedef struct Directory {
    char name;
    int size;
    d_entry *entries;      //should initialize this way (dynamically allocate) d_entry * entries

} Directory;




