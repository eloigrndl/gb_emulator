/**
 * @file timer.h
 * @brief Game Boy Timer simulation header
 *
 * @author Eloi Garandel, Erik Wengle, EPFL
 * @date 2020
 */

#include <stdint.h>
#include <stdio.h>

#include "component.h"
#include "bit.h"
#include "cpu.h"
#include "bus.h"
#include "timer.h"
#include "cpu-storage.h"
#include "error.h"
#include "gameboy.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Returns state of a timer
 *
 * @param timer timer to check
 * @return state of timer
 */
bit_t timer_state(gbtimer_t* timer);

/**
 * @brief Increases timer if state has changed
 *
 * @param timer timer to check
 * @param old_state previous state
 * @return error code
 */
int timer_incr_if_state_change(gbtimer_t* timer, bit_t old_state);



// ==== see timer.h ========================================
int timer_init(gbtimer_t* timer, cpu_t* cpu){
    M_REQUIRE_NON_NULL(timer);
    M_REQUIRE_NON_NULL(cpu);
    
    timer->cpu = cpu;
    timer->counter = 0;

    return ERR_NONE;
}


// ==== see timer.h ========================================
int timer_cycle(gbtimer_t* timer){

    M_REQUIRE_NON_NULL(timer);
    M_REQUIRE_NON_NULL(timer->cpu);

    bit_t old_state = timer_state(timer);
    timer->counter += GB_TICS_PER_CYCLE;
    uint8_t msb = msb8(timer->counter);

    M_REQUIRE_NO_ERR(cpu_write_at_idx(timer->cpu, REG_DIV, msb));
    M_REQUIRE_NO_ERR(timer_incr_if_state_change(timer, old_state));
    return ERR_NONE;
}


// ==== tool method ========================================
bit_t timer_state(gbtimer_t* timer){
    M_REQUIRE_NON_NULL(timer);
    M_REQUIRE_NON_NULL(timer->cpu);

    data_t current_state = cpu_read_at_idx(timer->cpu, REG_TAC);
    
    bit_t TAC_bit = bit_get(current_state, 2);
    uint8_t two_lsb = current_state & 0x3;

    switch(two_lsb){
        case 0: return TAC_bit & bit_get(msb8(timer->counter), 1);
        default:
                return TAC_bit & bit_get(timer->counter, 2 * two_lsb + 1);
    }
}

// ==== see timer.h ========================================
int timer_bus_listener(gbtimer_t* timer, addr_t addr){
	M_REQUIRE_NON_NULL(timer);
    M_REQUIRE_NON_NULL(timer->cpu);

    switch(addr){
        case REG_DIV:   timer->counter = 0;
                        M_REQUIRE_NO_ERR(cpu_write_at_idx(timer->cpu, REG_DIV, 0));
        case REG_TAC:
						M_REQUIRE_NO_ERR(timer_incr_if_state_change(timer, timer_state(timer)));
        default:        return ERR_NONE;
    }
    
    return ERR_NONE;
}

// ==== tool method ========================================
int timer_incr_if_state_change(gbtimer_t* timer, bit_t old_state){
    M_REQUIRE_NON_NULL(timer);
    M_REQUIRE_NON_NULL(timer->cpu);

    if(0 != old_state && timer_state(timer) == 0){
        
        uint8_t current_timer = cpu_read_at_idx(timer->cpu, REG_TIMA);

        if(current_timer == 0xFF){
            M_REQUIRE_NO_ERR(cpu_write_at_idx(timer->cpu, REG_TIMA, cpu_read_at_idx(timer->cpu, REG_TMA)));
            cpu_request_interrupt(timer->cpu, TIMER);
        } else {
            M_REQUIRE_NO_ERR(cpu_write_at_idx(timer->cpu, REG_TIMA, current_timer + 1));
        }
        
    }
    return ERR_NONE;
}

#ifdef __cplusplus
}
#endif
