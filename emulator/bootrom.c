/**
 * @file bootrom.c
 * @brief Game Boy Boot ROM
 * @author E. Garandel, E. Wengle EPFL
 * @date 2020
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

// ==== see bootrom.h ========================================
int bootrom_init(component_t* c){
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NO_ERR(component_create(c, MEM_SIZE(BOOT_ROM)));

    data_t data[] = GAMEBOY_BOOT_ROM_CONTENT;

    for(int i = 0; i < MEM_SIZE(BOOT_ROM); i++){
        c->mem->memory[i] = data[i];
    }

    return ERR_NONE;
}

// ==== see bootrom.h ========================================
int bootrom_bus_listener(gameboy_t* gameboy, addr_t addr){
    M_REQUIRE_NON_NULL(gameboy);
   
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
