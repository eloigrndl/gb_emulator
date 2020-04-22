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
    if(c == NULL || c->mem == NULL || c->mem->memory == NULL)
        return ERR_BAD_PARAMETER;


    if(c->end - c->start + offset >= c->mem -> size)
        return ERR_ADDRESS;
    

    for(int i = 0; i < c->mem -> size - offset; i++){
        bus[c-> start+i] = &(c->mem->memory[offset+i]);
        
    }

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
    if(c == NULL || end < start || end - start >= BUS_SIZE || c->mem == NULL )
        return ERR_BAD_PARAMETER;
    
    //TODO: verification for end, start and offset!!
    //TODO write Tests
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
    if(c == NULL || end < start || end - start >= BUS_SIZE || c->mem == NULL){
        return ERR_BAD_PARAMETER;
    }


    if(end - start >= c-> mem->size) 
        return ERR_ADDRESS;



    //TODO: check for other possible errors
    //checks whether bus is free for this space
    for(int i = start; i <= end; i++){
        if(bus[i] != NULL)
            return ERR_ADDRESS;
    }

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
    if(c == NULL){
        return ERR_BAD_PARAMETER;
    }

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
    if(data16 == NULL)
        return ERR_BAD_PARAMETER;

    data_t lsb = 0;
    data_t msb = 0;

    error_code e1 = bus_read(bus, address, &lsb);       //TODO: this will not return 0xFF but 0xFFFF in case both invalid
    error_code e2 = bus_read(bus, address+1, &msb);     //FIXME: lsb and msb address order?

    if(e1 != ERR_NONE)
        return e1;
    if(e2 != ERR_NONE)
        return e2;

    *data16 = merge8(lsb, msb); 

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
    if(bus[address] == NULL) 
        return ERR_BAD_PARAMETER;
    
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
    if(bus[address] == NULL || bus[address+1] == NULL)
        return ERR_BAD_PARAMETER;

    bus_write(bus, address,  lsb8(data16));         
    bus_write(bus, address+1, msb8(data16));
    return ERR_NONE;
}



