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
#ifndef _FSDIR_H
#define _FSDIR_H
#include "fsLow.h"
#include <stdint.h>
#include <sys/stat.h>
#include <stdbool.h>
#define MAX_NUM_ENTRIES 100
#define MAX_NAME_LEN 30

//directory should contain array of entries 
typedef struct d_entry {                           
    //need field that is location where file is stored
    ino_t d_ino;                // file serial # --> offset field? 
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
    d_entry entries[MAX_NUM_ENTRIES];
    //d_entry *entries;      //should initialize this way (dynamically allocate) d_entry * entries

} Directory;




#endif