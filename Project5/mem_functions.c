#include "mem.h"
extern BLOCK_HEADER* first_header;

int Get_Size(BLOCK_HEADER header){
    return header.size_alloc & 0xFFFFFFFE;
}

int Is_Free(BLOCK_HEADER header){
    if((header.size_alloc & 1) == 0){
	return 0;
    }
    return -1;
}

int Get_Padding_Size(int size){
    int padding = 0;

    if(((size + 8) % 16) != 0) {
	padding = 16 - ((size + 8) % 16);
    }

    return padding;
}

// return a pointer to the payload
// if a large enough free block isn't available, return NULL
void* Mem_Alloc(int size){
    // find a free block that's big enough
    int alloc;
    int block_size;
    int padding;
    int old_size;
    int new_size;
    void* user_pointer;
    BLOCK_HEADER *current = first_header;
    BLOCK_HEADER *split;

    if(size > 1592){
	return NULL;
    }

    while(1) {
	alloc = Is_Free(*current);
	block_size = Get_Size(*current);

	if(block_size >= size && alloc == 0){
	     //Allocate
	     //Save old size
	     current -> payload = size;
	     old_size = current -> size_alloc;

	     //Update Padding
	     padding = Get_Padding_Size(size);

	     //Update size
	     current -> size_alloc = (current -> payload) + padding + 8;
	     new_size = current -> size_alloc;

	     //Set allocated
	     current -> size_alloc = current -> size_alloc + 1;

	     //if new size - old size is over 16 it needs to be split
	     if((old_size - new_size) >= 16){
		//Split
		split = (BLOCK_HEADER *)((unsigned long)current + new_size);
		split -> size_alloc = old_size - new_size;
		split -> payload = split -> size_alloc - 8;
	     }

	     //return user_pointer
	     user_pointer = (void *)((unsigned long)current + 8);
	     return user_pointer;
	}

	//Last Block
	if (current->size_alloc == 1) {
	     return NULL;
	}

	//Next Block
	current = (BLOCK_HEADER *)((unsigned long)current + block_size);
    }

    return NULL;
}


// return 0 on success
// return -1 if the input ptr was invalid
int Mem_Free(void *ptr){
    if(ptr == NULL){
	return -1;
    }
    // traverse the list and check all pointers to find the correct block 
    // if you reach the end of the list without finding it return -1
    // Traverse and check if block matches Get_User_Pointer functions
    BLOCK_HEADER *current = first_header;
    BLOCK_HEADER *previous = NULL;
    int block_size;
    void *user_input;

    //traversal
    while(1){
	block_size = Get_Size(*current);
	user_input = (void *)((unsigned long)current + 8);

	//Free Block
	if(user_input == ptr){
	   //Set alloc bit to 0
	   if((current -> size_alloc & 1) == 0){
	   }
	   else{
	     current->size_alloc = current->size_alloc - 1;
	     current -> payload = (current -> payload) + Get_Padding_Size(current -> payload);

	     //Coallesce
	     for(int i = 0; i < 2; i++){
		  current = first_header;
		  previous = current;
	          while(current -> size_alloc != 1){
		     block_size = Get_Size(*current);
		     current = (BLOCK_HEADER *)((unsigned long)current + block_size);
		     if(((current -> size_alloc & 1) == 0) && ((previous -> size_alloc & 1) == 0)){
		        int var = current -> size_alloc;
			current -> size_alloc = 0;
			current -> payload = 0;

		        previous -> size_alloc = (previous -> size_alloc) + var;
			block_size = Get_Size(*previous);
		        previous -> payload = block_size - 8;
			break;
		     }
		     previous = current;
	          }
	     }
	     return 0;
	   }
	}

	current = (BLOCK_HEADER *)((unsigned long)current + block_size);
    }

    //return failure
    return -1;
}

