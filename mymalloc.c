#include <malloc.h> // We bring in the old malloc function
#include <stdio.h>  // Any other headers we need here
          // NOTE: You should remove malloc.h, and not include <stdlib.h> in your final implementation.

#include <debug.h> // definition of debug_printf
#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>


struct block_desc {
  struct block_desc *next;
  size_t s;
  int free;
} block_desc_t;

block_desc_t *head = NULL;

void *create_memory(size_t s) {
  void *ptr = sbrk(0);
  void *request = sbrk(s + sizeof(struct block_desc));
  assert(request == ptr);
  if (request == (void *) -1) {
    return NULL;
  }
  block_desc_t* curr_block_desc = (struct block_desc*)ptr;
  curr_block_desc->next = NULL;
  curr_block_desc->s = s;
  curr_block_desc->free = 0; 

  return ptr;
}

void *mymalloc(size_t s) {
  if (s <= 0) {
    return NULL;
  }

  void* next_block;
  if (head == NULL) {
    next_block = create_memory(s);
    head = next_block;
    return (void*)((char*)next_block + sizeof(struct block_desc));
  }

  block_desc_t *current = head;
  while (current) {
    if (current->size == s && current->free == 1) { 
      current_block->free = 0;
      return current_block + 1;
    }
    if (current->next == NULL) {
      break;
    }
    current = current->next;
  }

  next_block = create_memory(s);
  current->next = next_block;
  return (void*)((char*)next_block + sizeof(struct block_desc));
}


void *mycalloc(size_t nmemb, size_t s) {
  if(nmemb == 0 || s == 0){
    return NULL;
  }
  
  void* block = mymalloc(nmemb * s);
  char* ptr = (char*) block;
  
  int i;
  for (i = 0; i < nmemb * s; i++) {
    ptr[i] = 0;
  }

  return block;
}

void myfree(void *ptr) {
  if(!ptr){
    perror("Cannot free this block, baka.");
    return;
  }
  block_desc_t *current = ((block_desc_t *)ptr) - 1;
  current->free = 1;
}
