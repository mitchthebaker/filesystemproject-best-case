/**************************************************************
* Class:  CSC-415-01
* Name: Tania Nemeth
* Student ID: 920680290
* Project: File System Project
*
* File: b_io.c
*
* Description: Interface of basic I/O functions (Buffered I/O open, 
* read, write, seek, and close)
*
**************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "b_io.h"
#include "mfs.h"
#include "fsVCB.h"
#define MAXFCBS 20
#define B_CHUNK_SIZE 512
uint64_t buf_size;

//part of this struct is taken directly from my assignment 5 buffered write in b_io.c 
typedef struct b_fcb {
	int linuxFd;	//holds the systems file descriptor
	char * buf;		//holds the open file buffer for reading 
	int index;		//holds the current position in the buffer
	int buflen;		//holds how many valid bytes are in the buffer
	int blockCount;
	int lastBlockByteCount;
  ino_t d_ino[10];
} b_fcb;
	
b_fcb fcbArray[MAXFCBS];

int startup = 0;	//Indicates that this has not been initialized

//Method to initialize our file system from assignment 5 
void b_init ()
	{
        //init fcbArray to all free
        for (int i = 0; i < MAXFCBS; i++)
            {
            fcbArray[i].linuxFd = -1; //indicates a free fcbArray
            }
            
        startup = 1;
	}

//Method to get a free FCB element from assignment 5
int b_getFCB ()
	{
        for (int i = 0; i < MAXFCBS; i++)
            {
            if (fcbArray[i].linuxFd == -1)
                {
                fcbArray[i].linuxFd = -2; // used but not assigned
                return i;		//Not thread safe
                }
            }
        return (-1);  //all in use
        }

//taken from my (Tania) assignment 5 buffered write implementation of b_open with tweaks 
int b_open(char * filename, int flags)
{
	int fd;
	int returnFd;
	
	if (startup == 0) b_init();  //Initialize our system
	
	// lets try to open the file before I do too much other work
	//opne the file with the given flags & make sure the permissions are correct 
	//attribute: https://stackoverflow.com/questions/2245193/why-does-open-create-my-file-with-the-wrong-permissions
	fd = open(filename, flags, 0666);	// --> does this need to be changed for file system? 
	if (fd  == -1)
		return (-1);		//error opening filename

	returnFd = b_getFCB();				// get our own file descriptor
										// check for error - all used FCB's
	fcbArray[returnFd].linuxFd = fd;	// Save the linux file descriptor
	//	release mutex
	
	//allocate our read ops buffer
	fcbArray[returnFd].buf = malloc(B_CHUNK_SIZE);
	if (fcbArray[returnFd].buf  == NULL)
		{
		// very bad, we can not allocate our buffer
		close (fd);							// close linux file
		fcbArray[returnFd].linuxFd = -1; 	//Free FCB
		return -1;
		}
		
	fcbArray[returnFd].buflen = 0; 			// have not read anything yet
	fcbArray[returnFd].index = 0;			// have not read anything yet
	return (returnFd);						// all set
}

// Interface to for buffered read taken from Tania's assignemt 5 implementation 
// and modified to work for file system 
int b_read(int fd, char * buffer, int count)
{
  //*** TODO ***:  Write buffered read function to return the data and # bytes read
	//               You must use the Linux System Calls (read) and you must buffer the data
	//				 in 512 byte chunks. i.e. your linux read must be in B_CHUNK_SIZE
		
	int bytesRead;				// for our reads
	int bytesReturned;			// what we will return
	int part1, part2, part3;	// holds the three potential copy lengths
	int numberOfBlocksToCopy;	// holds the number of whole blocks that are needed
	int remainingBytesInMyBuffer;	// holds how many bytes are left in my buffer						
	
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}
		
	if (fcbArray[fd].linuxFd == -1)		//File not open for this descriptor
		{
		return -1;
		}	
		
	
	// number of bytes available to copy from buffer
	remainingBytesInMyBuffer = fcbArray[fd].buflen - fcbArray[fd].index;	
	
	// Part 1 is The first copy of data which will be from the current buffer
	// It will be the lesser of the requested amount or the number of bytes that remain in the buffer
	
	if (remainingBytesInMyBuffer >= count)  	//we have enough in buffer
		{
		part1 = count;		// completely buffered (the requested amount is smaller than what remains)
		part2 = 0;
		part3 = 0;			// Do not need anything from the "next" buffer
		}
	else
		{
		part1 = remainingBytesInMyBuffer;				//spanning buffer (or first read)
		
		// Part 1 is not enough - set part 3 to how much more is needed
		part3 = count - remainingBytesInMyBuffer;		//How much more we still need to copy
		
		// The following calculates how many 512 bytes chunks need to be copied to
		// the callers buffer from the count of what is left to copy
		numberOfBlocksToCopy = part3 / B_CHUNK_SIZE;  //This is integer math
		part2 = numberOfBlocksToCopy * B_CHUNK_SIZE; 
		
		// Reduce part 3 by the number of bytes that can be copied in chunks
		// Part 3 at this point must be less than the block size
		part3 = part3 - part2; // This would be equivalent to part3 % B_CHUNK_SIZE		
		}
				
	if (part1 > 0)	// memcpy part 1
		{
		memcpy (buffer, fcbArray[fd].buf + fcbArray[fd].index, part1);
		fcbArray[fd].index = fcbArray[fd].index + part1;
		}
		
	if (part2 > 0) 	//blocks to copy direct to callers buffer
		{
		bytesRead = read(fcbArray[fd].linuxFd, buffer+part1, numberOfBlocksToCopy*B_CHUNK_SIZE);
		part2 = bytesRead;  //might be less if we hit the end of the file
		
		// Alternative version that only reads one block at a time
		/******
		int tempPart2 = 0;
		for (int i = 0 i < numberOfBlocksToCopy; i++)
			{
			bytesRead = read (fcbArray[fd].linuxFd, buffer+part1+tempPart2, B_CHUNK_SIZE);
			tempPart2 = tempPart2 + bytesRead;
			}
		part2 = tempPart2;
		******/
		}
		
	if (part3 > 0)	//We need to refill our buffer to copy more bytes to user
		{		
		//try to read B_CHUNK_SIZE bytes into our buffer
		bytesRead = read(fcbArray[fd].linuxFd, fcbArray[fd].buf, B_CHUNK_SIZE);
		
		// we just did a read into our buffer - reset the offset and buffer length.
		fcbArray[fd].index = 0;
		fcbArray[fd].buflen = bytesRead; //how many bytes are actually in buffer
		
		if (bytesRead < part3) // not even enough left to satisfy read request from caller
			part3 = bytesRead;
			
		if (part3 > 0)	// memcpy bytesRead
			{
			memcpy (buffer+part1+part2, fcbArray[fd].buf + fcbArray[fd].index, part3);
			fcbArray[fd].index = fcbArray[fd].index + part3; //adjust index for copied bytes
			}	
		}
	bytesReturned = part1 + part2 + part3;
	return (bytesReturned);	
}

// Interface to for buffered write taken from Tania's assignemt 5 implementation 
// and modified to work for file system 
int b_write(int fd, char * buffer, int count)
	{
		int bytes_remaining_in_my_buffer;
		int bytes_written;
		int bytes_remaining_in_user_buffer;
		int user_index; 	//index in user buffer
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS))
		{
		return (-1); 					//invalid file descriptor
		}
		
	if (fcbArray[fd].linuxFd == -1)		//File not open for this descriptor
		{
		return -1;
		}	

	// Part 1 is what can be filled from the current buffer, which may or may not be enough

	//figuring out how many more bytes we can put in our buffer
	bytes_remaining_in_my_buffer = B_CHUNK_SIZE - fcbArray[fd].index;
	
	//part 1
	if(bytes_remaining_in_my_buffer > count){
		//copying the buffer into our write buffer at the current index 
		memcpy(fcbArray[fd].buf + fcbArray[fd].index, buffer, count);
		//incrementing the index by how much we wrote
		fcbArray[fd].index = fcbArray[fd].index + count;
		return count;
	}
	//copying data in user buffer into write buffer to fill it up 
	memcpy(fcbArray[fd].buf+fcbArray[fd].index, buffer, bytes_remaining_in_my_buffer);
	//now that the buffer is full, we are writing the whole buffer to the file 
	write(fcbArray[fd].linuxFd, fcbArray[fd].buf, B_CHUNK_SIZE);
	bytes_remaining_in_user_buffer = count - bytes_remaining_in_my_buffer;	//so we don't lose track of count
	user_index = bytes_remaining_in_my_buffer;
	
	//part 2
	while(bytes_remaining_in_user_buffer > B_CHUNK_SIZE){
		//copying 512 bytes from user buffer into the write buffer 
		memcpy(fcbArray[fd].buf, buffer + user_index, B_CHUNK_SIZE);

		LBAwrite()
		write(fcbArray[fd].linuxFd, fcbArray[fd].buf, B_CHUNK_SIZE);
		//increment user index by 512 bytes
		user_index += B_CHUNK_SIZE;
		bytes_remaining_in_user_buffer -= B_CHUNK_SIZE;
	}

	//part 3 
	memcpy(fcbArray[fd].buf, buffer + user_index, bytes_remaining_in_user_buffer);
	//update index into fcb array to the number of bytes we just copied
	fcbArray[fd].index = bytes_remaining_in_user_buffer;
	return count;

	
	}

// int b_seek (int fd, off_t offset, int whence)
// {

// }    


// Interface to for buffered close taken from Tania's assignemt 5 implementation 
// and modified to work for file system 
void b_close(int fd)
	{
        write(fcbArray[fd].linuxFd, fcbArray[fd].buf, fcbArray[fd].index);		//writing out the rest of what's in our buffer from part 3
        close (fcbArray[fd].linuxFd);		// close the linux file handle
        free (fcbArray[fd].buf);			// free the associated buffer
        fcbArray[fd].buf = NULL;			// Safety First
        fcbArray[fd].linuxFd = -1;			// return this FCB to list of available FCB's 
	}
	

void b_io_write(int fd, char *buf, int SIZE) {
	struct VCB *vcb = malloc(512);
  getVCB(vcb);

	int bytes_remaining_in_my_buffer;
	int bytes_written;
	int bytes_remaining_in_user_buffer;
	int user_index; 	//index in user buffer
	if (startup == 0) b_init();  //Initialize our system

	// check that fd is between 0 and (MAXFCBS-1)
	if ((fd < 0) || (fd >= MAXFCBS)) {
		return (-1); 					//invalid file descriptor
	}
	//File not open for this descriptor
	if (fcbArray[fd].linuxFd == -1)	{
		return -1;
	}	
	bytes_remaining_in_my_buffer = B_CHUNK_SIZE - fcbArray[fd].index;
	
	//part 1
	if(bytes_remaining_in_my_buffer > count){
		// copying the buffer into our write buffer at the current index 
		memcpy(fcbArray[fd].buf+fcbArray[fd].index, buffer, count);
		// incrementing the index by how much we wrote
		fcbArray[fd].index = fcbArray[fd].index + count;
		LBAwrite(fcbArray[fd].buf, 1, fcbArray[fd].d_ino);
		return count;

	}
	//copying data in user buffer into write buffer to fill it up 
	memcpy(fcbArray[fd].buf+fcbArray[fd].index, buffer, bytes_remaining_in_my_buffer);
	//now that the buffer is full, we are writing the whole buffer to the file 
	
	LBAwrite(fcbArray[fd].buf, 1, fcbArray[fd].d_ino);
	bytes_remaining_in_user_buffer = count - bytes_remaining_in_my_buffer;	//so we don't lose track of count
	user_index = bytes_remaining_in_my_buffer;
	
	//part 2
	while(bytes_remaining_in_user_buffer > B_CHUNK_SIZE){
		//copying 512 bytes from user buffer into the write buffer 
		memcpy(fcbArray[fd].buf, buffer + user_index, B_CHUNK_SIZE);
		fcbArray[fd].blockCount += 1;
		fcbArray[fd].d_ino[blockCount] = requestFSBlocks(vcb, 1);
		LBAwrite(fcbArray[fd].buf, 1, fcbArray[fd].d_ino[fcbArray[fd].blockCount]);
		//increment user index by 512 bytes
		user_index += B_CHUNK_SIZE;
		bytes_remaining_in_user_buffer -= B_CHUNK_SIZE;
	}

	//part 3 
	memcpy(fcbArray[fd].buf, buffer + user_index, bytes_remaining_in_user_buffer);
	//update index into fcb array to the number of bytes we just copied
	fcbArray[fd].index = bytes_remaining_in_user_buffer;
	return count;
}
