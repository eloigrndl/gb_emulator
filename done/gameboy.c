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

    M_REQUIRE_NO_ERR(component_create(&(gameboy -> components[0]), MEM_SIZE(WORK_RAM)));  //TODO replace with M_REQUIRE_NO_ERR
    M_REQUIRE_NO_ERR(cbus_plug(gameboy -> bus, &(gameboy -> components[0]), WORK_RAM_START, WORK_RAM_END));

    component_t* echo_ram = calloc(1, sizeof(component_t)); //FIXME: where to free??

    M_REQUIRE_NO_ERR(component_create(echo_ram, 0));
    M_REQUIRE_NO_ERR(component_shared(echo_ram, &(gameboy -> components[0])));
    M_REQUIRE_NO_ERR(bus_plug(gameboy -> bus, echo_ram, ECHO_RAM_START, ECHO_RAM_END));
    return ERR_NONE;
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
    //FIXME: need to unplug cpu from bus?
    cpu_free(&(gameboy->cpu));
}

/**
 * @brief Runs a gamefor for/until a given cycle
 */
int gameboy_run_until(gameboy_t* gameboy, uint64_t cycle){

}



#ifdef __cplusplus
}
#endif
