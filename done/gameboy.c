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
    
    memset(&(gameboy -> bus), NULL, sizeof(bus_t));  //FIXME: Need to memset the BUS_SIZE elements?

    error_code e2 = component_create(&(gameboy -> components[0]), MEM_SIZE(WORK_RAM));
    if(e2 != ERR_NONE)
        return e2;

    error_code e3 = bus_plug(gameboy -> bus, &(gameboy -> components[0]), WORK_RAM_START, WORK_RAM_END);
    if(e3 != ERR_NONE)
        return e3;

    component_t* echo_ram = calloc(1, sizeof(component_t));
    error_code e4 = component_create(echo_ram, 0);
    if(e4 != ERR_NONE)
        return e4;

    error_code e5 = component_shared(echo_ram, &(gameboy -> components[0]));
    if(e5 != ERR_NONE)
        return e5;
    
    error_code e6 = bus_plug(gameboy -> bus, echo_ram, ECHO_RAM_START, ECHO_RAM_END);
    return e6;
}

/**
 * @brief Destroys a gameboy
 *
 * @param gameboy pointer to gameboy to destroy
 */
void gameboy_free(gameboy_t* gameboy){
    for(int i = 0; i < GB_NB_COMPONENTS; ++i){
        component_free(&(gameboy -> components[i]));
    }
    //FIXME: do we have to free the pointer itself ?
}

/**
 * @brief Runs a gamefor for/until a given cycle
 */
int gameboy_run_until(gameboy_t* gameboy, uint64_t cycle){

}



#ifdef __cplusplus
}
#endif
