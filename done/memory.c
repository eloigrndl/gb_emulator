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

/**
 * @brief Creates memory structure
 *
 * @param mem memory structure pointer to initialize
 * @param size size of the memory to create
 * @return error code
 */
int mem_create(memory_t* mem, size_t size){
    if(mem == NULL || size == 0){
        return ERR_BAD_PARAMETER;
    }

    mem->memory = calloc(size, sizeof(data_t));
    if(mem-> memory == NULL){
        return ERR_MEM;
    }

    mem->size = size;
    return ERR_NONE;
}

/**
 * @brief Destroys memory structure
 *
 * @param mem memory structure pointer to destroy
 */
void mem_free(memory_t* mem){
    free(mem->memory);
    mem -> memory = NULL;   //TODO : is = NULL necessary ?
    mem -> size = 0;
}

#ifdef __cplusplus
}
#endif
