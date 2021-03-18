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


// ==== see component.h ========================================
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


// ==== see component.h ========================================
int component_shared(component_t* c, component_t* c_old){
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NON_NULL(c_old);
    M_REQUIRE_NON_NULL(c_old->mem);
    M_REQUIRE_NON_NULL(c_old->mem->memory);

    c->mem = c_old->mem;
    c->start = 0;
    c->end = 0;

    return ERR_NONE; 
}


// ==== see component.h ========================================
void component_free(component_t* c){
    if(c == NULL)
        return;
        
    c->start = 0;
    c->end = 0;

    if(c->mem == NULL) 
        return;
    
    mem_free(c->mem);
    free(c->mem);
    c->mem = NULL;

    return;
}
