#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MEMSIZE 80 //pretending whole memory is an array of 80 characters

typedef struct block{
    int size;
    char process; //
    int startIndex;
    struct block* next;
    bool free; //to see if it is free or not
} block; //this is saying struct block is the same as block

void allocateBlock(int size, char process, block* correctPosition);
void bestFitAllocate(int size, char process);
void worstFitAllocate(int size, char process);
void firstFitAllocate(int size, char process);
void freeBlock(char process); //we are freeing a whole process which can be spread out between non contiguous blocks not just one variable. 
void show(); //prints out whole array
void compact();
void readBlock(char* fileName);
