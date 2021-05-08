/**************************************************************
* Class:  CSC-415
* Name: Professor Bierman
* Student ID: N/A
* Project: Basic File System
*
* File: mfs.h
*
* Description: 
*	This is the file system interface.
*	This is the interface needed by the driver to interact with
*	your filesystem.
*
**************************************************************/
#ifndef _MFS_H
#define _MFS_H
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include "fsDir.h"
#include "b_io.h"

#include <dirent.h>
#define FT_REGFILE	DT_REG
#define FT_DIRECTORY DT_DIR
#define FT_LINK	DT_LNK

#ifndef uint64_t
typedef u_int64_t uint64_t;
#endif
#ifndef uint32_t
typedef u_int32_t uint32_t;
#endif


struct fs_diriteminfo
	{
	ino_t d_ino;				// Directory item inode
    unsigned short d_reclen;    // Length of this record
    unsigned char fileType;     // The directory item's file type
    char d_name[256]; 			// Filename max filename is 255 characters
	};


typedef struct
	{
	unsigned short  d_reclen;		   // Length of this record
	struct fs_diriteminfo * entries;   // Directory entries
	unsigned short numEntries;		   // Total number of directory entries
	unsigned short	curEntry;	       // Which directory entry position; like file pos 
	} fdDir;

// Initialize our file system
// Pass in volumeSize and blockSize for formatting the volume
int fs_init(uint64_t volumeSize, uint64_t blockSize);

// Make a new directory
// For parameters, pass in the directory name and a mode if any
int fs_mkdir(const char *pathname, mode_t mode);

// Remove a directory
// For parameters, pass in the directory name to remove
int fs_rmdir(const char *pathname);

// Move a directory in the file system
// For parameters, pass in the directory's old path and its new path
int fs_mv(char * oldPath, char * newPath);

// Open a directory in the file system
// For parameters, pass in the directory's name
fdDir * fs_opendir(const char *name);

// Read a directory in the file system
// For parameters, pass in the directory to be read 
struct fs_diriteminfo *fs_readdir(fdDir *dirp);

// Close a directory in the file system
// For parameters, pass in the directory to be closed
int fs_closedir(fdDir *dirp);

// Get the current working directory in the file system (pwd)
char * fs_getcwd(char *buf, size_t size);

// Set the current working directory in the file system (cd)
int fs_setcwd(char *buf);   //linux chdir
int fs_isFile(char * path);	//return 1 if file, 0 otherwise
int fs_isDir(char * path);		//return 1 if directory, 0 otherwise
int fs_delete(char* filename);	//removes a file
int fs_close();

struct fs_stat
	{
	off_t     st_size;    		/* total size, in bytes */
	blksize_t st_blksize; 		/* blocksize for file system I/O */
	blkcnt_t  st_blocks;  		/* number of 512B blocks allocated */
	time_t    st_accesstime;   	/* time of last access */
	time_t    st_modtime;   	/* time of last modification */
	time_t    st_createtime;   	/* time of last status change */
	
	/* add additional attributes here for your file system */
	};

int fs_stat(const char *path, struct fs_stat *buf);

// Temporary storage for vcb, current directory, and block size
int temp_init();

#endif

