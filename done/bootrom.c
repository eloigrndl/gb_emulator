/**
 * @file bootrom.c
 * @brief Game Boy Boot ROM
 * @author Eloi Garandel, Erik Wengle EPFL
 * @date 05/2020
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "bus.h"
#include "component.h"
#include "gameboy.h"
#include "bootrom.h"
#include "error.h"
#include <stdio.h>

/**
 * @brief Writes bootrom content to a component
 *
 * @param c component to write the bootrom content to
 * @return error code
 */
int bootrom_init(component_t* c){
    M_REQUIRE_NO_ERR(component_create(c, MEM_SIZE(BOOT_ROM)));

    data_t data[] = GAMEBOY_BOOT_ROM_CONTENT;

    //TODO fix this
    for(int i = 0; i < BOOT_ROM_END - BOOT_ROM_START + 1; i++){
        c->mem->memory[i] = data[i];
    }

    return ERR_NONE;
}

/**
 * @brief Bootrom bus listening handler
 *
 * @param gameboy gameboy
 * @param address trigger address
 * @return error code
 */
int bootrom_bus_listener(gameboy_t* gameboy, addr_t addr){
    M_REQUIRE_NON_NULL(gameboy);
   
   //FIXME: find out what arg "addr" is for
    if(REG_BOOT_ROM_DISABLE == addr && gameboy->boot == 1){
        M_REQUIRE_NO_ERR(bus_unplug(gameboy->bus, &(gameboy->bootrom)));
        M_REQUIRE_NO_ERR(cartridge_plug(&(gameboy->cartridge), gameboy->bus));
        gameboy->boot = 0;
    }
    return ERR_NONE;
}

#ifdef __cplusplus
}
#endif
