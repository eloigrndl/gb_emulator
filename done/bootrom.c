#pragma once

/**
 * @file bootrom.c
 * @brief Game Boy Boot ROM
 * @author Eloi Garandel, Eric Wengle EPFL
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

/**
 * @brief Writes bootrom content to a component
 *
 * @param c component to write the bootrom content to
 * @return error code
 */
int bootrom_init(component_t* c){
    M_REQUIRE_NO_ERR(component_create(c, MEM_SIZE(BOOT_ROM)));
    (c->memory)->memory = GAMEBOY_BOOT_ROM_CONTENT; //FIXME only first one is okay or have to copy each cell
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
    M_REQUIRE_NO_NULL(gameboy);
    M_REQUIRE_NO_NULL(gameboy->cpu);
    if((gameboy->cpu)->write_listener == addr){
        M_REQUIRE_NO_ERR(bus_unplug(gameboy->bus, &(gameboy->bootrom)));
        //FIXME cartridge_plug()
        gameboy->boot = 0;
    }
    return ERR_NONE;
}

#ifdef __cplusplus
}
#endif
