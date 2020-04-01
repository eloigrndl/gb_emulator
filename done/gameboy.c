/**
 * @file gameboy.c
 * @brief Gameboy Body for GameBoy Emulator
 *
 * @author E. Wengle, E. Garandel, EPFL
 * @date 2020
 */

#include <stdint.h>
#include <stdlib.h>
#include "error.h"

#include "gameboy.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates a gameboy
 *
 * @param gameboy pointer to gameboy to create
 */
int gameboy_create(gameboy_t* gameboy, const char* filename){
    if(gameboy == NULL || filename == NULL)
        return ERR_BAD_PARAMETER;

        component_t work_ram;
        
        bus_plug(&work_ram, WORK_RAM_START, WORK_RAM_END);
}

/**
 * @brief Destroys a gameboy
 *
 * @param gameboy pointer to gameboy to destroy
 */
void gameboy_free(gameboy_t* gameboy){

}

/**
 * @brief Runs a gamefor for/until a given cycle
 */
int gameboy_run_until(gameboy_t* gameboy, uint64_t cycle){

}



#ifdef __cplusplus
}
#endif
