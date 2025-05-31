//////////////////////////////////////////////////////////////////////////////////
//
//   Program Name:  Buddy System Algorithm
//                  
//   Description:  Buddy System Algorithm
//                  
//   Student name: Zak Turner
//
//
//////////////////////////////////////////////////////////////////////////////////


#ifndef __BUDDYSYS_H__
#define __BUDDYSYS_H__


#include "auxiliary.h"

extern long long int MEMORYSIZE;


// The minimum block size 
// should be set greater 
// than the size of the 
// header to accommodate 
// some useful data.


#define MIN_BLOCK_SIZE 64//bytes - this is the minimum size of a block that can be allocated including the header
#define MIN_ROW_SIZE 6 //This is the minimum row size, which is 2^6 = 64 bytes. This is the minimum block size that can be allocated.
#define MAX_POSSIBLE_ROWS 64 //This number has been allocated as the compiler was not happy with the number of rows being calculated during runtime.

typedef unsigned char byte;// shorter, replace cast to (char *) with cast to (byte *)

struct llist { long long int size;   //size of the block (ONLY for data, this size does not consider the Node size! (so it is same as s[k])
               int alloc;  //0 is free, 1 means allocated
               struct llist * next; //next component
               struct llist * previous; //previous component
};
typedef struct llist Node;

// extern int requested_memory; // this is the memory request of n bytes
// extern int memory_request_max = requested_memory + sizeof(Node); 

extern Node *wholememory;//Pointer to the Biggest chunk
extern Node *FreeList[MAX_POSSIBLE_ROWS];
extern int Max_Rows; //the maximum number of rows in the MemoryArray based on MAX and MIN block size

////////////////////////////////////////////////////////////////////////////
// Function declarations
int getRows(int max, int min);
void initBuddySys();
void *buddyMalloc(int request_memory); 
int buddyFree(void *p);

// Helper functions
int powerOfTwo(int k);
int smallestK(int memory_request_max);
void headRemoval(int k);

#endif