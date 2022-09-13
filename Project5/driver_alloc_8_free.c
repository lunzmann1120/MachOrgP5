#include "mem.h"
#include <stdio.h>

int main() {
    Initialize_Memory_Allocator(1600);
    Mem_Dump();
   
    int *p = Mem_Alloc(1593);

    Mem_Free(p);
    Mem_Dump();
    
    Free_Memory_Allocator();
    return 0;
}
