/**
 * @file gameboy.c
 * @brief Gameboy Body for GameBoy Emulator
 *
 * @author E. Wengle, E. Garandel, EPFL
 * @date 2020
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "error.h"
#include "util.h"
#include "bootrom.h"

#include "gameboy.h"

#ifdef __cplusplus
extern "C" {
#endif


    /**
     * @brief Listens for writes on bus address used by blargg
     * @param gameboy gameboy to listen at
     * @param addr trigger address
     * @return error code
     * 
     */
    #ifdef BLARGG
        int blargg_bus_listener(gameboy_t* gameboy, addr_t addr){
            M_REQUIRE_NON_NULL(gameboy);

            if(BLARGG_REG == addr){
                printf("%c", *(gameboy->bus[BLARGG_REG]));
            }
            return ERR_NONE;
        }
    #endif

// ==== see gameboy.h ========================================
    int gameboy_create(gameboy_t* gameboy, const char* filename){
        M_REQUIRE_NON_NULL(gameboy);
        M_REQUIRE_NON_NULL(filename);
        
        zero_init_ptr(gameboy);
        zero_init_var(gameboy->bus);


        M_REQUIRE_NO_ERR(component_create(&(gameboy->components[0]), MEM_SIZE(WORK_RAM)));  
        M_REQUIRE_NO_ERR(bus_plug(gameboy->bus, &(gameboy->components[0]), WORK_RAM_START, WORK_RAM_END));
        

        component_t* echo_ram = NULL;
        M_EXIT_IF_NULL(echo_ram = calloc(1, sizeof(component_t)), sizeof(component_t));
        M_REQUIRE_NO_ERR(component_create(echo_ram, 0));
        M_REQUIRE_NO_ERR(component_shared(echo_ram, &(gameboy->components[0])));
        ++gameboy->nb_components;

        echo_ram->mem->size = MEM_SIZE(ECHO_RAM);
        M_REQUIRE_NO_ERR(bus_plug(gameboy->bus, echo_ram, ECHO_RAM_START, ECHO_RAM_END));
        free(echo_ram);

        M_REQUIRE_NO_ERR(component_create(&(gameboy->components[1]), MEM_SIZE(REGISTERS)));  
        M_REQUIRE_NO_ERR(bus_plug(gameboy->bus, &(gameboy->components[1]), REGISTERS_START, REGISTERS_END));
        ++gameboy->nb_components;
        
        M_REQUIRE_NO_ERR(component_create(&(gameboy->components[2]), MEM_SIZE(EXTERN_RAM)));  
        M_REQUIRE_NO_ERR(bus_plug(gameboy->bus, &(gameboy->components[2]), EXTERN_RAM_START, EXTERN_RAM_END));
        ++gameboy->nb_components;
        
        M_REQUIRE_NO_ERR(component_create(&(gameboy->components[3]), MEM_SIZE(VIDEO_RAM)));  
        M_REQUIRE_NO_ERR(bus_plug(gameboy->bus, &(gameboy->components[3]), VIDEO_RAM_START, VIDEO_RAM_END));
        ++gameboy->nb_components;
        
        M_REQUIRE_NO_ERR(component_create(&(gameboy->components[4]), MEM_SIZE(GRAPH_RAM)));  
        M_REQUIRE_NO_ERR(bus_plug(gameboy->bus, &(gameboy->components[4]), GRAPH_RAM_START, GRAPH_RAM_END));
        ++gameboy->nb_components;

        M_REQUIRE_NO_ERR(component_create(&(gameboy->components[5]), MEM_SIZE(USELESS)));  
        M_REQUIRE_NO_ERR(bus_plug(gameboy->bus, &(gameboy->components[5]), USELESS_START, USELESS_END));
        ++gameboy->nb_components;

        M_REQUIRE_NO_ERR(cpu_init(&(gameboy->cpu)));
        M_REQUIRE_NO_ERR(cpu_plug(&(gameboy->cpu), &(gameboy->bus)));
        
        gameboy->DIV = 0;
        gameboy->TAC = 0;
        gameboy->TIMA = 0;
        gameboy->TMA = 0;
        (*(gameboy->cpu.bus))[REG_DIV] = &(gameboy->DIV);
        (*(gameboy->cpu.bus))[REG_TAC] = &(gameboy->TAC);
        (*(gameboy->cpu.bus))[REG_TIMA] = &(gameboy->TIMA);
        (*(gameboy->cpu.bus))[REG_TMA] = &(gameboy->TMA);
        
        M_REQUIRE_NO_ERR(lcdc_init(gameboy));
        M_REQUIRE_NO_ERR(lcdc_plug(&(gameboy->screen), gameboy->bus));

        M_REQUIRE_NO_ERR(joypad_init_and_plug(&(gameboy->pad), &(gameboy->cpu)));

        M_REQUIRE_NO_ERR(timer_init(&(gameboy->timer), &(gameboy->cpu)));
        
        M_REQUIRE_NO_ERR(cartridge_init(&(gameboy->cartridge), filename));
        M_REQUIRE_NO_ERR(cartridge_plug(&(gameboy->cartridge), gameboy->bus));

        gameboy->boot = 1;
        M_REQUIRE_NO_ERR(bootrom_init(&(gameboy->bootrom)));
        M_REQUIRE_NO_ERR(bootrom_plug(&(gameboy->bootrom), gameboy->bus));
        gameboy->cpu.IME = 1;


        return ERR_NONE;
    }

    // ==== see gameboy.h ========================================
    void gameboy_free(gameboy_t* gameboy){
        if(gameboy == NULL) return;
        
        for(int i = 0; i < gameboy->nb_components; ++i){
            bus_unplug(gameboy->bus, &(gameboy->components[i]));
            component_free(&(gameboy->components[i]));
        }

        for(int i = ECHO_RAM_START; i <= ECHO_RAM_END; ++i)
            gameboy->bus[i] = NULL;
        
        gameboy->nb_components = 0;
        cpu_free(&(gameboy->cpu));
        cartridge_free(&(gameboy->cartridge));
        component_free(&(gameboy->bootrom)); 
        lcdc_free(&(gameboy->screen));       
    }


    // ==== see gameboy.h ========================================
    int gameboy_run_until(gameboy_t* gameboy, uint64_t cycle){
        M_REQUIRE_NON_NULL(gameboy);
        
        while(gameboy->cycles < cycle){
           M_REQUIRE_NO_ERR(lcdc_cycle(&(gameboy->screen), gameboy->cycles));
           M_REQUIRE_NO_ERR(timer_cycle(&(gameboy->timer)));
           M_REQUIRE_NO_ERR(cpu_cycle(&(gameboy->cpu)));
           gameboy->cycles++;

           M_REQUIRE_NO_ERR(bootrom_bus_listener(gameboy, (gameboy->cpu).write_listener));
           M_REQUIRE_NO_ERR(lcdc_bus_listener(&(gameboy->screen), (gameboy->cpu).write_listener));
           M_REQUIRE_NO_ERR(timer_bus_listener(&(gameboy->timer), (gameboy->cpu).write_listener));
           M_REQUIRE_NO_ERR(joypad_bus_listener(&(gameboy->pad), (gameboy->cpu).write_listener));

            #ifdef BLARGG
                M_REQUIRE_NO_ERR(blargg_bus_listener(gameboy, (gameboy->cpu).write_listener));
            #endif
           
        }
        return ERR_NONE;
    } 


#ifdef __cplusplus
}
#endif
