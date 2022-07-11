#include <string.h>
#include "stack.hpp"
#include <unistd.h>

// the malloc inspired by https://danluu.com/malloc-tutorial/

using namespace std;


struct dataBlock {
    struct dataBlock *next;
    int free;
    size_t size;
};

dataBlock *head = NULL; //HEAD

struct dataBlock *serchBlock(struct dataBlock **last, size_t size) {
    struct dataBlock *current = head;
    while (current && (current->free == 0  || current->size < size)) {
        *last = current;
        current = current->next;
    }
    return current;    //// last ends pointing to the 'block meta' of the last memory allocation if cuurent is null
}

struct dataBlock *createMemory(struct dataBlock* last, size_t size) {
    struct dataBlock *block;
    block = static_cast<dataBlock *>(sbrk(0));  // points to current creak adress and later points to the start of the new allocated memory , beacuse of the // casting block points to tnly the sizeof(block meta) first bytes of the memory allocated
    void *request = sbrk(size + sizeof(struct dataBlock));
    if (last!=NULL) { // NULL on first request.
        last->next = block;   /// next points to the start of the memory block
        block->size = size;
        block->next = NULL;
        block->free = 0;
    }
    if (request == (void *) -1) {
        return NULL; //
    }
    return block;
}


void *malloc(size_t size) {
    struct dataBlock *block;
    if (size <= 0) {
        return NULL;
    }
    else {
        struct dataBlock *lastElement = head;
        block = serchBlock(&lastElement, size);  // noticed we change where lastElement points to beacuse we past &lastElement and not lastElement
        if (!block) { // Failed to find free block - block poins to Null and lastElement points to the lastElement block meta of the lastElement allocation
            block = createMemory(lastElement, size); // pointer to the new memory block
            if (!block) {
                return NULL;
            }
        } else {      // Found free block
            block->free = 0;
        }
    }
    if (!head) { // there is noting insdie the linkidlist
        block = createMemory(NULL, size);
        if (!block) {
            return NULL;
        }
        head = block;
    }
    block++;
    return(block);  // we want to skip the 'sizeof(block meta) first bytes' and return the adress of the begining of the allocated memory
}

// explenation : ask the mount of bytes we need for dynemick aloocation and after we get get that memory by the malloc function , bzero all the bytes
void *calloc(size_t blocksNumber, size_t bytesEachBlock) {
    size_t size = blocksNumber * bytesEachBlock;
    void *ptr = malloc(size);
    bzero(ptr,size);
    return ptr;
}

void free(void *ptr) {
    if (!ptr) {
        return;
    }
    struct dataBlock* block_ptr = (struct dataBlock*)ptr;  /// go to the block meta - the data of the memory aloocated that we want to free
    block_ptr--;
    block_ptr->free = 1;                                       // decleare the memory allocated is free to use for other works.
}







//##############
    static node *ptop = NULL;
    void push(char *str_input) {
        node *newnode = (node *) malloc(sizeof(node));
        strcpy(newnode->str, str_input);
        newnode->next = ptop;
        ptop = newnode;
    }
    void pop() {
        if (ptop == NULL)
            return;
        else {
            node *ptr = ptop;
            ptop = ptop->next;
            free(ptr);
        }
    }
    char *top() {
        if (ptop == NULL)
            return NULL;
        return (ptop->str);
    }