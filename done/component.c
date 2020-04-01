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
#include "memory.h"

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
    component_t* c_new  = calloc(1, sizeof(component_t)); //TODO use new pointer or reject null values? 
    if(c_new == NULL)
        return ERR_MEM;

    if(mem_size == 0){
        c_new->mem = NULL;
    }else{
        c_new->mem = calloc(1, sizeof(memory_t)); //FIXME: with or without *
        error_code e = mem_create(c_new->mem, mem_size);
        if(e != ERR_NONE){
            printf("VOILA LE BUG\n");
             return e;
        }
    }
    c_new->start = 0;
    c_new->end = 0;
    c = c_new;
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

    c-> start = 0;
    c-> end = 0;

    free(c);
    c = NULL;    

    return;
}


#ifdef __cplusplus
}
#endif
