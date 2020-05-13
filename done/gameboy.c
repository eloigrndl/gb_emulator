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
        
        echo_ram->mem->size = MEM_SIZE(ECHO_RAM);
        M_REQUIRE_NO_ERR(bus_plug(gameboy->bus, echo_ram, ECHO_RAM_START, ECHO_RAM_END));   //TODO intermediate gameboy
        free(echo_ram);

        M_REQUIRE_NO_ERR(component_create(&(gameboy->components[1]), MEM_SIZE(REGISTERS)));  
        M_REQUIRE_NO_ERR(bus_plug(gameboy->bus, &(gameboy->components[1]), REGISTERS_START, REGISTERS_END));
        ++gameboy->nb_connected;
        
        M_REQUIRE_NO_ERR(component_create(&(gameboy->components[2]), MEM_SIZE(EXTERN_RAM)));  
        M_REQUIRE_NO_ERR(bus_plug(gameboy->bus, &(gameboy->components[2]), EXTERN_RAM_START, EXTERN_RAM_END));
        ++gameboy->nb_connected;
        
        M_REQUIRE_NO_ERR(component_create(&(gameboy->components[3]), MEM_SIZE(VIDEO_RAM)));  
        M_REQUIRE_NO_ERR(bus_plug(gameboy->bus, &(gameboy->components[3]), VIDEO_RAM_START, VIDEO_RAM_END));
        ++gameboy->nb_connected;
        
        M_REQUIRE_NO_ERR(component_create(&(gameboy->components[4]), MEM_SIZE(GRAPH_RAM)));  
        M_REQUIRE_NO_ERR(bus_plug(gameboy->bus, &(gameboy->components[4]), GRAPH_RAM_START, GRAPH_RAM_END));
        ++gameboy->nb_connected;

        M_REQUIRE_NO_ERR(component_create(&(gameboy->components[5]), MEM_SIZE(USELESS)));  
        M_REQUIRE_NO_ERR(bus_plug(gameboy->bus, &(gameboy->components[5]), USELESS_START, USELESS_END));
        ++gameboy->nb_connected;

        M_REQUIRE_NO_ERR(cpu_init(&(gameboy->cpu)));
        M_REQUIRE_NO_ERR(cpu_plug(&(gameboy->cpu), &(gameboy->bus)));
        ++gameboy->nb_connected;
        
        
        
        gameboy->DIV = 0;
        gameboy->TAC = 0;
        gameboy->TIMA = 0;
        gameboy->TMA = 0;
        (*(gameboy->cpu.bus))[REG_DIV] = &(gameboy->DIV);
        (*(gameboy->cpu.bus))[REG_TAC] = &(gameboy->TAC);
        (*(gameboy->cpu.bus))[REG_TIMA] = &(gameboy->TIMA);
        (*(gameboy->cpu.bus))[REG_TMA] = &(gameboy->TMA);
        

        M_REQUIRE_NO_ERR(timer_init(&(gameboy->timer),&(gameboy->cpu)));
        
        M_REQUIRE_NO_ERR(cartridge_init(&(gameboy->cartridge), filename));
        M_REQUIRE_NO_ERR(cartridge_plug(&(gameboy->cartridge), gameboy->bus));

        gameboy->boot = 1;
        M_REQUIRE_NO_ERR(bootrom_init(&(gameboy->bootrom)));
        M_REQUIRE_NO_ERR(bootrom_plug(&(gameboy->bootrom), gameboy->bus));
        
        //FIXME: how to plug cartridge correctly?
        //FIXME: seg fault??

        return ERR_NONE;
    }

    // ==== see gameboy.h ========================================
    void gameboy_free(gameboy_t* gameboy){
        if(gameboy == NULL) return;

        for(int i = 0; i < gameboy->nb_connected; ++i){
            component_free(&(gameboy->components[i]));
        }


        cpu_free(&(gameboy->cpu));
        cartridge_free(&(gameboy->cartridge)); //TODO: need to free other stuff too?
        component_free(&(gameboy->bootrom));
        
        //FIXME: need to unplug differently?
        for(int i = 0; i < BUS_SIZE; i++){
            gameboy->bus[i] = NULL;
        }
    }


    // ==== see gameboy.h ========================================
    int gameboy_run_until(gameboy_t* gameboy, uint64_t cycle){
        M_REQUIRE_NON_NULL(gameboy);
        while(gameboy->cycles < cycle){
            /*
           printf("========================================\n"); fflush(stdout);
           printf("Running cycle %ld \n", gameboy->cycles); fflush(stdout);
           printf("Current byte: %X \n", *(gameboy->bus[gameboy->cycles]));
           printf("PC = %d\n",gameboy->cpu.PC);   
            */

           M_REQUIRE_NO_ERR(timer_cycle(&(gameboy->timer)));
           M_REQUIRE_NO_ERR(cpu_cycle(&(gameboy->cpu)));
           M_REQUIRE_NO_ERR(bootrom_bus_listener(gameboy, (gameboy->cpu).write_listener));
           gameboy->cycles++;

            //just for the test
            if(gameboy->cycles%17556 == 5) 
                cpu_request_interrupt(&(gameboy->cpu), VBLANK);
         
        }
        return ERR_NONE;
    } 



#ifdef __cplusplus
}
#endif
