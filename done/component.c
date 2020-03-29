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
    if(c == NULL){
        return ERR_BAD_PARAMETER;
    }


    if(mem_size == 0){
        c -> mem = NULL;
    } else {
        c -> mem = calloc(mem_size, sizeof(memory_t));
        if(c -> mem == NULL)
            //if calloc fails
            return ERR_ADDRESS;
    }
        
    (c->mem) -> size = mem_size;
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
    //TODO: check if correct ?
    c -> mem = c_old -> mem;
    c_old -> start = 0;
    c_old -> end = 0;
}


/**
 * @brief Destroy's a component
 *
 * @param c component pointer to destroy
 */
void component_free(component_t* c){
    free(c -> mem)
    c -> mem = NULL;
    c->start = 0;
    c-> end = 0;
}


#ifdef __cplusplus
}
#endif
