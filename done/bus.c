#pragma once

/**
 * 
 * //TODO: edit description
 * @file component.c
 * @brief Bit operations for GameBoy Emulator
 *
 * @author E. Wengle, E. Garandel, EPFL
 * @date 2020
 */

#include "bus.h"
#include "error.h"
#include "bit.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Remap the memory of a component to the bus
 *
 * @param bus bus to remap to
 * @param c component to remap
 * @param offset new offset to use
 * @return error code
 */
int bus_remap(bus_t bus, component_t* c, addr_t offset){
    //TODO: verify also other arguments

    if(c->end - c->start + offset < c->mem.size){
        return ERR_ADDRESS;
    }


    for(int i = 0; i < c->mem.size - offset; i++){
        bus[c-> start+i] = &c->mem.memory[offset+i];
        
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
    if(c == NULL){
        return ERR_BAD_PARAMETER;
    }

    if(end < start){
        return ERR_ADDRESS;
    }

    //TODO: verify args as before

    error_code e = bus_remap(bus, c, offset);

    //FIXME please
    if(e == ERR_NONE){
        c->start = start;
        c->end = end;
    }else{
        c->start = 0;
        c->end = 0;
    }
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
    if(c == NULL){
        return ERR_BAD_PARAMETER;
    }
    //TODO: check for other possible errors
    //checks whether bus is free for this space
    for(int i = start; i <= end; i++){
        if(bus[i] != NULL)
            return ERR_ADDRESS;
    }

    error_code e = bus_remap(bus, c, 0);

    //TODO: react upon e != 0
    return e;
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
    //TODO: check for arg validity


    for(int i = c-> start; i <= c->end; i++)
        bus[i] = NULL;

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
    //TODO verify args

    if(bus[address] == NULL){
        *data = 0xFF;
        return ERR_NONE;
    }else{
        *data = *bus[address];
        //TODO treat other errors
        return ERR_NONE;
    }

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
    //TODO: verify args

    data_t lsb = 0;
    data_t msb = 0;

    bus_read(bus, address, &lsb);
    bus_read(bus, address+1, &msb);

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
    //TODO verify args
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
    //TODO verify args

    *bus[address] = lsb8(data16);
    *bus[address] = msb8(data16);
    return ERR_NONE;
}


#ifdef __cplusplus
}
#endif
