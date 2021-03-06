/**************************************************************
* Class:  CSC-415-01
* Name: Tania Nemeth
* Student ID: 920680290
* Project: File System Project
*
* File: b_io.h
*
* Description: Interface of basic I/O functions
*
**************************************************************/

#ifndef _B_IO_H
#define _B_IO_H
#include <fcntl.h>

int b_open (char * filename, int flags);
int b_read (int fd, char * buffer, int count);
//int b_io_read(int fd, char * buffer, int count);
int b_write (int fd, char * buffer, int count);
//int b_io_write(int fd, char *buf, int SIZE);
int b_seek (int fd, off_t offset, int whence);
void b_close (int fd);

#endif

