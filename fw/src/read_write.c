#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "read_write.h"
#include "utils_function.h"


// file:        read_write.c
// owner:       shahar
// description: module to read and write to logic unit.
// comment:     currntly support write / read  through os call. later by template can switch to mem mapping


int write_to_logic(uint32_t data, uint64_t address){
    verb_print(MED, "DEBUG | enter write_to_logic with : data = %d, address = %x\n", data, address);
    // varaible
    uint32_t read_data = 0x0000;
    char read_data_as_string[11]; //todo switch to define //8 hex symbols + 2 prefix + 1 null char
    char addr_as_string[ADDRESS_WIDTH_HEX]; 
    char cmd[100]; //todo review if enough for the cmd length. //todo switch to defines
    memset(addr_as_string, 0, ADDRESS_WIDTH_HEX);
    memset(cmd, 0, 100); //todo switch to defines
    convert_hex_to_string(address, addr_as_string);
    verb_print(HIGH, "DEBUG | convert_hex_to_string = %s\n", addr_as_string);
    // write data to logic
    sprintf(cmd, "monitor %s %d", addr_as_string, data);
    verb_print(MED, "DEBUG | running cmd = %s\n", cmd);
    system(cmd);

    // read data from address.
    // todo shahar, switch the following implementation with read function instead. (save code lines and debuging)
    memset(cmd, 0, 100); //todo switch to defines
    sprintf(cmd, "monitor %s", addr_as_string);
    verb_print(MED, "DEBUG | running cmd = %s\n", cmd);
    read_data = system(cmd); //todo review what is the reutn type
    verb_print(HIGH, "DEBUG | returning value from above cmd = %s, read_data = %d\n", cmd, read_data);
    if (read_data == data){
        verb_print(HIGH, "DEBUG | read_data equal to data asked to be written\n");
        return TRUE;
    }
    verb_print(HIGH, "DEBUG | read_data not equal to data asked to be written\n");
    return FALSE;
//    read_data = system("monitor 0x41200000 21474836");
//    system("monitor 0x41200000 21474836");
}

uint32_t read_from_logic(uint32_t address){
    verb_print(MED, "DEBUG | enterd read_from_logic with addr = %x\n", address);
    //variables
    uint32_t read_data = 0x0000;
    char read_data_as_string[11]; //todo switch to define //8 hex symbols + 2 prefix + 1 null char
    char addr_as_string[ADDRESS_WIDTH_HEX]; 
    char cmd[100]; //todo review if enough for the cmd length. //todo switch to defines
    memset(addr_as_string, 0, ADDRESS_WIDTH_HEX);
    memset(cmd, 0, 100); //todo switch to defines
    convert_hex_to_string(address, addr_as_string);
    sprintf(cmd, "monitor %s", addr_as_string);
    verb_print(MED, "DEBUG | running cmd = %s\n", cmd);
    read_data = system(cmd); //todo review what is the reutn type
    verb_print(HIGH, "DEBUG | returning value from above cmd = %s, read_data = %d\n", cmd, read_data);
    return read_data;
}