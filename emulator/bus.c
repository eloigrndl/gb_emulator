/**
 * @file bus.c
 * @brief Game Boy Bus Emulator
 *
 * @author E. Wengle, E. Garandel, EPFL
 * @date 2020
 */

#include "bus.h"
#include "error.h"
#include "bit.h"
#include "myMacros.h"
#include <stdio.h>

// ==== see bus.h ========================================
int bus_remap(bus_t bus, component_t* c, addr_t offset){
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NON_NULL(c->mem);
    M_REQUIRE_NON_NULL(c->mem->memory);
    M_REQUIRE(c-> end - c->start + offset < c->mem->size, ERR_ADDRESS, "Memory size %lu too small", c->mem->size);


    for(int i = 0; i < c->mem->size - offset; i++){
        bus[c->start+i] = &(c->mem->memory[offset+i]);
    }
        
    return ERR_NONE;
}

// ==== see bus.h ========================================
int bus_forced_plug(bus_t bus, component_t* c, addr_t start, addr_t end, addr_t offset){
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE((end >= start) && (end - start < BUS_SIZE), ERR_ADDRESS, "End %u and Start %u invalid", end, start);
    
    c->start = start;
    c->end = end;

    M_EXIT_IF_ERR_DO_SOMETHING(bus_remap(bus, c, offset), c->start = 0; c->end = 0);
    return ERR_NONE;
}


// ==== see bus.h ========================================
int bus_plug(bus_t bus, component_t* c, addr_t start, addr_t end){
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NON_NULL(c->mem);
    M_REQUIRE((end >= start) && (end - start < BUS_SIZE) && (end - start < c->mem->size), ERR_ADDRESS, "End %u and Start %u invalid or Memory too small", start, end);


    for(int i = start; i <= end; i++){
        M_REQUIRE(bus[i] == NULL, ERR_ADDRESS, "Part of area already occupied: %X", i);
    }

    return bus_forced_plug(bus, c, start, end, 0);
}


// ==== see bus.h ========================================
int bus_unplug(bus_t bus, component_t* c){
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(c);

    for(int i = c->start; i <= c->end; i++)
        bus[i] = NULL;
    
    c->start = 0;
    c->end = 0;

    return ERR_NONE;
}


// ==== see bus.h ========================================
int bus_read(const bus_t bus, addr_t address, data_t* data){
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(data);
    M_REQUIRE(address < BUS_SIZE, ERR_ADDRESS, "Address %X too big", address);

    *data = bus[address] == NULL ? DEFAULT_READ_VALUE : *(bus)[address];
    return ERR_NONE;
}


// ==== see bus.h ========================================
int bus_read16(const bus_t bus, addr_t address, addr_t* data16){
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(data16);
    *data16 = (address >= 0xFFFF || bus[address] == NULL || bus[address+1] == NULL) ? DEFAULT_READ_VALUE : merge8(*(bus)[address], *(bus)[address+1]); 
    return ERR_NONE;
}


// ==== see bus.h ========================================
int bus_write(bus_t bus, addr_t address, data_t data){
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(bus[address]);
    
    *bus[address] = data;
    return ERR_NONE;
}


// ==== see bus.h ========================================
int bus_write16(bus_t bus, addr_t address, addr_t data16){
    M_REQUIRE_NON_NULL(bus);
    M_REQUIRE_NON_NULL(bus[address]);
    M_REQUIRE_NON_NULL(bus[address+1]);
    M_REQUIRE(address < 0xFFFF, ERR_ADDRESS, "Address %X out of bounds", address);

    bus_write(bus, address,  lsb8(data16));         
    bus_write(bus, address+1, msb8(data16));
    return ERR_NONE;
}



