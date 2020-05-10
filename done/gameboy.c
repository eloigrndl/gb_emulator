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
#include "util.h"

#include "gameboy.h"

#ifdef __cplusplus
extern "C" {
#endif

// ==== see gameboy.h ========================================
    int gameboy_create(gameboy_t* gameboy, const char* filename){
        M_REQUIRE_NON_NULL(gameboy);
        M_REQUIRE_NON_NULL(filename);
        
        zero_init_var(gameboy->bus);

        M_REQUIRE_NO_ERR(component_create(&(gameboy->components[0]), MEM_SIZE(WORK_RAM)));  
        M_REQUIRE_NO_ERR(bus_plug(gameboy->bus, &(gameboy->components[0]), WORK_RAM_START, WORK_RAM_END));

        component_t* echo_ram = NULL;
        M_EXIT_IF_NULL(echo_ram = calloc(1, sizeof(component_t)), sizeof(component_t));
        M_REQUIRE_NO_ERR(component_create(echo_ram, 0));
        M_REQUIRE_NO_ERR(component_shared(echo_ram, &(gameboy->components[0])));
        M_REQUIRE_NO_ERR(bus_plug(gameboy->bus, echo_ram, ECHO_RAM_START, ECHO_RAM_END));   //TODO intermediate gameboy

        M_REQUIRE_NO_ERR(component_create(&(gameboy->components[1]), MEM_SIZE(REGISTERS)));  
        M_REQUIRE_NO_ERR(bus_plug(gameboy->bus, &(gameboy->components[1]), REGISTERS_START, REGISTERS_END));

        M_REQUIRE_NO_ERR(component_create(&(gameboy->components[2]), MEM_SIZE(EXTERN_RAM)));  
        M_REQUIRE_NO_ERR(bus_plug(gameboy->bus, &(gameboy->components[2]), EXTERN_RAM_START, EXTERN_RAM_END));

        M_REQUIRE_NO_ERR(component_create(&(gameboy->components[3]), MEM_SIZE(VIDEO_RAM)));  
        M_REQUIRE_NO_ERR(bus_plug(gameboy->bus, &(gameboy->components[3]), VIDEO_RAM_START, VIDEO_RAM_END));

        M_REQUIRE_NO_ERR(component_create(&(gameboy->components[4]), MEM_SIZE(GRAPH_RAM)));  
        M_REQUIRE_NO_ERR(bus_plug(gameboy->bus, &(gameboy->components[4]), GRAPH_RAM_START, GRAPH_RAM_END));

        M_REQUIRE_NO_ERR(component_create(&(gameboy->components[5]), MEM_SIZE(USELESS)));  
        M_REQUIRE_NO_ERR(bus_plug(gameboy->bus, &(gameboy->components[5]], USELESS_START, USELESS_END));

        M_REQUIRE_NO_ERR(cpu_init(&(gameboy->cpu)));
        M_REQUIRE_NO_ERR(cpu_plug(&(gameboy->cpu), &(gameboy->bus)));
        return ERR_NONE;
    }

// ==== see gameboy.h ========================================
void gameboy_free(gameboy_t* gameboy){
    for(int i = 0; i < GB_NB_COMPONENTS; ++i){
        component_free(&(gameboy->components[i]));
    }
}


// ==== see gameboy.h ========================================
int gameboy_run_until(gameboy_t* gameboy, uint64_t cycle){
    while(gameboy->cycles < cycle){
        cpu_cycle(gameboy->cpu);
        //TODO: how to increment????????????????!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    }
    return 0;
}



#ifdef __cplusplus
}
#endif
