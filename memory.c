#include "memory.h"

char RAM[81];//array holding all 80 processes
int numBlocks = 1;


//does setting head to null just set all its variables to null?
block* head; //always going to have one empty block until RAM is full
//have to initialize size and free in main as it currently doesnt have allocated memory for those variables

int main(){
    head = malloc(sizeof(block));
    head->size = 80;
    head->free = true;
    for(int i = 0; i < 80; i++){
        RAM[i] = '.'; //default value before a process is added
    }
    RAM[80] = '\0'; //null terminator (how the system knows where the string ends)
    readBlock("test.txt");
}

void readBlock(char* fileName){
    FILE* instructionFile = fopen(fileName, "r"); //returns file pointer
    char* instructions = malloc(sizeof(char) * 20); //length of each string set to 20 each string is a line in the input file
    //fgets(where to put strings, lenght of string, whole file) reads file line by line
    while(fgets(instructions, 20, instructionFile) != NULL){
        //printf("%s\n", RAM);
        char* instructionType = strtok_r(instructions, " \t\n", &instructions); //each string in line seperated by space
        char* p = strtok_r(instructions, " \t\n", &instructions);
        char* sz = strtok_r(instructions, " \t\n", &instructions);
        char* algorithm = strtok_r(instructions, " \t\n", &instructions);
        int size = -1; //default case if the instruction does not have size, not changed
        char process = '.';
        if(p != NULL){
            process = p[0];
        }
        if(sz != NULL){
            size = atoi(sz);
        }
        if(instructionType[0] == 'S'){
            show();
        }
        else if(instructionType[0] == 'C'){
            compact();
        }
        else if(instructionType[0] == 'F'){
            freeBlock(process);
        }
        else if(instructionType[0] == 'E'){
            break;
        }
        else if(algorithm[0] == 'F'){
            firstFitAllocate(size, process);
        }
        else if(algorithm[0] == 'B'){
            bestFitAllocate(size, process);
        }
        else if(algorithm[0] == 'W'){
            worstFitAllocate(size, process);
        }
        //printf("%s %d %d %s\n", instructionType, process, size, algorithm);
    }
}

void allocateBlock(int size, char process, block* correctPosition){
    if(correctPosition != NULL){
        if(correctPosition->size == size){
            correctPosition->process = process;
            correctPosition->free = false;
        }
        else{
            block* newBlock = malloc(sizeof(block));
            newBlock->startIndex = correctPosition->startIndex + size;
            newBlock->free = true;
            newBlock->next = correctPosition->next;
            newBlock->size = correctPosition->size - size;
            correctPosition->size = size;
            correctPosition->free = false;
            correctPosition->process = process;
            correctPosition->next = newBlock;
        }
        for(int i = correctPosition->startIndex; i < correctPosition->startIndex + size; i++){
            RAM[i] = process;
        }
    }
}

//iterates from head node to end and returns block with the smallest space big enough for size
void bestFitAllocate(int size, char process){ 
    block* curr = head;
    block* correctPosition = NULL;
    while(curr != NULL){
        if(curr->free == true && correctPosition == NULL && curr->size >= size){
            correctPosition = curr;
        }
        else if(curr->free == true && curr->size >= size && correctPosition != NULL && correctPosition->size > curr->size){
            correctPosition = curr;
        }
        curr = curr->next;
    }
    if(correctPosition == NULL){
        printf("No available space for process %d needing size %d\n", process, size);
    }
    else{
        allocateBlock(size, process, correctPosition);
    }
}

/*Starting form the head node, traverses the linked list and allocates the first block with sufficient size to the passed in process.
Faster in the best and average case than bestFit to find the correct block, however could leave undesirably sized blocks available*/
void firstFitAllocate(int size, char process){
    block* curr = head;
    block* correctPosition = NULL;
    while(curr != NULL){
        if(curr->free && curr->size >= size){
            correctPosition = curr;
            break;
        }
        curr = curr->next;
    }
    if(correctPosition == NULL){
        printf("No available space for process %d needing size %d\n", process, size);
    }
    else{
        allocateBlock(size, process, correctPosition);
    }
}

/*Traverses entire linked list starting at head to find the position with the most available space to set the process. The advantage is that
there will be large internal fragmentation so some other smaller processes can take the place in the left over partition. Same speed as best fit*/
void worstFitAllocate(int size, char process){
    block* curr = head;
    block* correctPosition = NULL;
    while(curr != NULL){
        if(curr->free && correctPosition == NULL && curr->size >= size){
            correctPosition = curr;
        }
        else if(curr->free && correctPosition != NULL && correctPosition->size < curr->size){
            correctPosition = curr;
        }
        curr = curr->next;
    }
    
    if(correctPosition == NULL){
        printf("No available space for process %d needing size %d\n", process, size);
    }
    else{
        allocateBlock(size, process, correctPosition);
    }
}

/*Merges free blocks together. Checks to see if there are any free blocks around the block we are freeing, and merges these together to one block.
Called at the end of freeBlock*/
void merge(){
    block* curr = head;
    while(curr != NULL && curr->next != NULL){
        if(curr->free && curr->next->free){
            block* nxt = curr->next->next;
            curr->size = curr->size + curr->next->size;
            free(curr->next);
            curr->next = nxt;
        }
        else{
            curr = curr->next;
        }
    }
}

void compact(){
    block* curr = head;
    block** prev = &head; //double pointer is adress of the pointer and we can update the value of the adress
    int count = 0; //counts the number of free spaces to allocate to end of list
    int index = 0;
    while(curr != NULL){
        block* deletedNode = NULL;
        if(curr->free){
            count+= curr->size;
            deletedNode = curr;
            *prev = curr->next;
            //printf("Free Block count = %d\n", count);
        }
        else{
            curr->startIndex = index;
            index += curr->size;
            for(int i = curr->startIndex; i < curr->startIndex + curr->size; i++){
                RAM[i] = curr->process;
            }
            //printf("Taken Block Start Index = %d, Size = %d\n", curr->startIndex, curr->size);
        }
        prev = &(curr->next);
        curr = curr->next;
        free(deletedNode);
    }
    block* endNode = malloc(sizeof(block));
    endNode->size = count;
    endNode->free = true;
    endNode->startIndex = index;
    endNode->next = NULL;
    endNode->process = '.';
    *prev = endNode;
    for(int i = endNode->startIndex; i < endNode->startIndex + endNode->size; i++){
        RAM[i] = endNode->process;
    }
}

/*free's all blocks with the passed in porcess. Traverses the linked list, and if the process is equal to the process passed through,
sets free to true and changes all of its indices within the RAM to '.' which indicates free space*/
void freeBlock(char process){
    block* curr = head;
    while(curr != NULL){
        if(curr->process == process){
            curr->free = true;
            curr->process = '.'; //dont do null with character
            for(int i = curr->startIndex; i < curr->startIndex + curr->size; i++){
                RAM[i] = '.';
            }
        }
        curr = curr->next;
    }
    merge();
}

//prints whole string representing the RAM
void show(){
    printf("%s\n", RAM);
}
