/**
 * @file memory.h
 * @brief Memory for Gamemu
 *
 * @author E. Wengle, E. Garandel, EPFL
 * @date 2020
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "memory.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

// ==== see memory.h ========================================
int mem_create(memory_t* mem, size_t size){
    M_REQUIRE_NON_NULL(mem);
    M_REQUIRE(size > 0, ERR_BAD_PARAMETER, "Size too small", size);
    M_EXIT_IF_NULL(mem->memory = calloc(size, sizeof(data_t)), sizeof(data_t));

    mem->size = size;
    return ERR_NONE;
}

// ==== see memory.h ========================================
void mem_free(memory_t* mem){
    if(mem == NULL)
        return;
    mem->size = 0;
     
    if(mem->memory == NULL) 
        return;

    free(mem->memory);
    mem -> memory = NULL;
    mem -> size = 0;
    return;
}

#ifdef __cplusplus
}
#endif
