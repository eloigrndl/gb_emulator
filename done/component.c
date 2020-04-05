/**
 * @file component.c
 * @brief Game Boy Component simulation header
 *
 * @author E. Wengle, E. Garandel, EPFL
 * @date 2020
 */

#include "error.h"
#include "component.h"
#include "memory.h"
#include <stdio.h>


/**
 * @brief Creates a component given various arguments
 *
 * @param c component pointer to initialize
 * @param mem_size size of the memory of the component
 * @return error code
 */
int component_create(component_t* c, size_t mem_size){
    if(c == NULL)
        return ERR_BAD_PARAMETER;

    if(mem_size == 0){
        c->mem = NULL;
    }else{
        c->mem = calloc(1, sizeof(memory_t)); //FIXME: with or without *
        if(c->mem == NULL) 
            return ERR_MEM;
        
        error_code e = mem_create(c->mem, mem_size);
        if(e != ERR_NONE){
             return e;
        }
    }
    c->start = 0;
    c->end = 0;


    
    return ERR_NONE;
} 

/**
 * @brief Shares memory between two components
 *
 * @param c component pointer to share to
 * @param c_old component to share from
 * @return error code
 */
int component_shared(component_t* c, component_t* c_old){
    //TODO: check if correct c_old vs c?
    if(c == NULL && c_old == NULL){
        return ERR_BAD_PARAMETER;
    }
    
    c -> mem = c_old -> mem;
    c -> start = 0;
    c -> end = 0;
    return ERR_NONE; 
}


/**
 * @brief Destroy's a component
 *
 * @param c component pointer to destroy
 */
void component_free(component_t* c){
    mem_free(c->mem);
    free(c->mem);
    c->mem = NULL;

    c-> start = 0;
    c-> end = 0;

    //FIXME: need to add TODO and FIXME?
    return;
}

//TODO ifndef?