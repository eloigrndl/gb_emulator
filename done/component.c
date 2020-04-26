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
    M_REQUIRE_NON_NULL(c);
    
    if(mem_size == 0){
        c->mem = NULL;
    }else{
        M_EXIT_IF_NULL(c->mem = calloc(1, sizeof(memory_t)), sizeof(memory_t)); 
        M_REQUIRE_NO_ERR(mem_create(c->mem, mem_size)); 
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
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NON_NULL(c_old);

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
    if(c == NULL || c->mem == NULL) return;
    
    mem_free(c->mem);
    free(c->mem);
    c->mem = NULL;

    c-> start = 0;
    c-> end = 0;

    return;
}
