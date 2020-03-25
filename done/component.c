#pragma once

/**
 * 
 * //TODO: edit description
 * @file component.c
 * @brief Bit operations for GameBoy Emulator
 *
 * @author E. Wengle, E. Garandel, EPFL
 * @date 2020
 */

#include "error.h"
#include "component.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates a component given various arguments
 *
 * @param c component pointer to initialize
 * @param mem_size size of the memory of the component
 * @return error code
 */
int component_create(component_t* c, size_t mem_size){
    if(mem_size > MAX_MEM_SIZE || c == NULL){
        return ERR_BAD_PARAMETER;
    }
    
    memset(c->mem.memory, 0, mem_size);             //TODO will memset really reset everything?
    
    c->mem.size = mem_size;
    c->start = 0;
    c->end = 0;
        

    return ERR_NONE;
}


/**
 * @brief Destroy's a component
 *
 * @param c component pointer to destroy
 */
void component_free(component_t* c){
    c->start = 0;
    c-> end = 0;
}


#ifdef __cplusplus
}
#endif
