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
#include <stdio.h>

/** 
 * @brief Remap the memory of a component to the bus
 *
 * @param bus bus to remap to
 * @param c component to remap
 * @param offset new offset to use
 * @return error code
 */
int bus_remap(bus_t bus, component_t* c, addr_t offset){
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NON_NULL(c->mem);
    M_REQUIRE_NON_NULL(c->mem->memory);
    M_REQUIRE(c-> end - c->start + offset < c->mem->size, ERR_ADDRESS, "Address error", ... );
    
    for(int i = 0; i < c->mem -> size - offset; i++)
        bus[c-> start+i] = &(c->mem->memory[offset+i]);
        
    return ERR_NONE;
}

/**
 * @brief Plug forcibly a component into the bus (allows to use outside of range start and end).
 *        For example, if we want to map a component to somewhere else than the initialy described area.
 *
 * @param bus bus to plug into
 * @param c component to plug into bus
 * @param start address from where to plug to (included)
 * @param end address until where to plug to (included)
 * @param offset offset where to start in the component
 * @return error code
 */
int bus_forced_plug(bus_t bus, component_t* c, addr_t start, addr_t end, addr_t offset){
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE((end >= start) && (end - start < BUS_SIZE), ERR_ADDRESS, "Address error", ...);
    
    error_code e = bus_remap(bus, c, offset);
    c-> start = (e == ERR_NONE) ? start : 0;
    c-> end = (e == ERR_NONE) ? end : 0;
    
    return e;
}


/**
 * @brief Plug a component into the bus
 *
 * @param bus bus to plug into
 * @param c component to plug into bus
 * @param start address from where to plug (included)
 * @param end address until where to plug (included)
 * @return error code
 */
int bus_plug(bus_t bus, component_t* c, addr_t start, addr_t end){
    
    M_REQUIRE_NON_NULL(c);
    M_REQUIRE_NON_NULL(c->mem);
    M_REQUIRE((end >= start) && (end - start < BUS_SIZE) && (end - start < c-> mem->size), ERR_ADDRESS, "Address error", ...);

    for(int i = start; i <= end; i++)
        M_REQUIRE(bus[i] == NULL, ERR_ADDRESS, "Part of area already occupied", ...);   //TODO find out about VA_ARGS

    return bus_forced_plug(bus, c, start, end, 0);
}


/**
 * @brief Unplug a component from the bus
 *
 * @param bus bus to unplug from
 * @param c component to plug into bus
 * @return error code
 */
int bus_unplug(bus_t bus, component_t* c){
    M_REQUIRE_NON_NULL(c);

    for(int i = c->start; i <= c->end; i++)
        bus[i] = NULL;
    
    c->start = 0;
    c->end = 0;

    return ERR_NONE;
}

/**
 * @brief Read the bus at a given address
 *
 * @param bus bus to read from
 * @param address address to read at
 * @param data pointer to write read data to
 * @return error code
 */
int bus_read(const bus_t bus, addr_t address, data_t* data){
    M_REQUIRE_NON_NULL(data);
    M_REQUIRE(address < BUS_SIZE, ERR_ADDRESS, "Address Error", ...);

    *data = bus[address] == NULL ? 0xFF : *(bus[address]);
    return ERR_NONE;
}

/**
 * @brief Read the bus at a given address (reads 16 bits)
 *
 * @param bus bus to read from
 * @param address address to read at
 * @param data16 pointer to write read data to
 * @return error code
 */
int bus_read16(const bus_t bus, addr_t address, addr_t* data16){
    M_REQUIRE_NON_NULL(data16);

    *data16 = (address == 0xFFFF || bus[address] == NULL || bus[address+1] == NULL) ? 0xFF : merge8(*(bus[address]), *(bus[address+1])); 
    return ERR_NONE;
}

/**
 * @brief Write to the bus at a given address
 *
 * @param bus bus to write to
 * @param address address to write at
 * @param data data to write
 * @return error code
 */
int bus_write(bus_t bus, addr_t address, data_t data){
    M_REQUIRE_NON_NULL(bus[address]);
    
    *bus[address] = data;
    return ERR_NONE;
}

/**
 * @brief Write to the bus at a given address (writes 16 bits)
 *
 * @param bus bus to write to
 * @param address address to write at
 * @param data16 data to write
 * @return error code
 */
int bus_write16(bus_t bus, addr_t address, addr_t data16){
    M_REQUIRE_NON_NULL(bus[address]);
    M_REQUIRE_NON_NULL(bus[address+1]);
    M_REQUIRE(address != 0xFFFF, ERR_ADDRESS, "Address error", ...);

    bus_write(bus, address,  lsb8(data16));         
    bus_write(bus, address+1, msb8(data16));
    return ERR_NONE;
}



