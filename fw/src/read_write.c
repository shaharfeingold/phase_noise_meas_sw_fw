#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "read_write.h"
#include "utils_function.h"
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "defines.h"
#include "fw_data_struct.h"
#include "error_handling.h"
// file:        read_write.c
// owner:       shahar
// description: module to read and write to logic unit.
// comment:     currntly support write / read  through os call. later by template can switch to mem mapping
//              need to support also catch the return code from the read/write.

int write_to_logic(uint32_t data, uint64_t address){
    verb_print(MED, "DEBUG | enter write_to_logic with : data = %d, address = %x\n", data, address);
    // varaible
    //int fd;
    void *ptr;
    char *name = "/dev/mem";
    uint32_t read_data = 0x0000;
    verb_print(HIGH, "DEBUG | open a fd to catch the read\n");
    int fd = open(name, O_RDWR);
    if (fd < 0) {
        handle_medium_error("Failed to open file descriptor in write_to_logic");
        return 1; 
    }
    verb_print(HIGH, "DEBUG | map the fd to memory address for the write and read\n");
    ptr = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, address); //each pointer of this map is width of 1B (memory is addrsable by bytes)
    if (ptr == MAP_FAILED) {
        close(fd);
        handle_medium_error("Memory mapping failed in write_to_logic");
        return FALSE;
    }
    // write data to logic
    verb_print(HIGH, "DEBUG | write data on addr 0x%x with the value %d\n", address, data);
    *((uint32_t *)(ptr)) = data;   
    verb_print(HIGH, "DEBUG | unmap memory to fd and close file descriptor\n");
    munmap(ptr, sysconf(_SC_PAGESIZE));
    close(fd);
    // read data from address.
    read_data = read_from_logic(address);
    if (read_data != data){
        handle_easy_error("Data read back does not match data written in write_to_logic");
        return FALSE;
    }
    verb_print(HIGH, "DEBUG | read_data equal to data asked to be written\n");
    return TRUE;
}

uint32_t read_from_logic(uint64_t address){
    verb_print(MED, "DEBUG | enterd read_from_logic with addr = %x\n", address);
    //variables
    //int fd;
    void *ptr;
    char *name = "/dev/mem";
    uint32_t read_data = 0x0000;
    verb_print(HIGH, "DEBUG | open a fd to catch the read\n");
    int fd = open(name, O_RDWR);
    if (fd < 0) {
        handle_medium_error("Failed to open file descriptor in read_from_logic");
        return 1; 
    }
    verb_print(HIGH, "DEBUG | map the fd to memory address for the read");
    ptr = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, address); //each pointer of this map is width of 1B (memory is addrsable by bytes)
    if (ptr == MAP_FAILED) {
        close(fd);
        handle_medium_error("Memory mapping failed in read_from_logic");
        return 0; // or another appropriate error code
    }
    read_data = *((uint32_t *)(ptr + 0)); //the output is in the base address of the memory mapping. 
    read_data = *((uint32_t *)(ptr + 0)); //the output is in the base address of the memory mapping.
    verb_print(HIGH, "DEBUG | Read data from logic = %x\n",read_data);
    verb_print(HIGH, "DEBUG | unmap memory to fd and close file descriptor\n");
    munmap(ptr, sysconf(_SC_PAGESIZE));
    close(fd);
    return read_data;
}

int read_from_array(DataArray* data_array){
    verb_print(MED, "DEBUG | enterd read_from_array\n");
    //variables
    //int fd;
    void *ptr;
    char *name = "/dev/mem";
    uint32_t read_data = 0x00000000;
    float read_data_float = 0x00000000;
    int index = 0;
    int offset = 0;
    verb_print(HIGH, "DEBUG | open a fd to catch the read\n");
     int fd = open(name, O_RDWR);
    if (fd < 0) {
        handle_medium_error("Failed to open file descriptor in read_from_array");
        return FALSE; 
    }
    verb_print(HIGH, "DEBUG | map the fd to memory address for the read");
    ptr = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, BUFFER_BASE_ADDR); //each pointer of this map is width of 1B (memory is addrsable by bytes)
    if (ptr == MAP_FAILED) {
        close(fd);
        handle_medium_error("Memory mapping failed in read_from_array");
        return FALSE; 
    }
    for (index = 0; index < data_array->TargetLen; index++){
        offset = index * 4;
        read_data = *((uint32_t *)(ptr + offset)); //the output is in the base address of the memory mapping. 
        read_data = *((uint32_t *)(ptr + offset)); //the output is in the base address of the memory mapping.
        verb_print(HIGH, "DEBUG | Read data from logic = %x\n",read_data);
                read_data_float = convert_fix_point_to_float(read_data);
        verb_print(HIGH, "DEBUG | Read data from logic as float = 0x%x | %f\n",read_data_float, read_data);
        // data_from_logic = read_from_logic(BUFFER_BASE_ADDR + offset);
        store_new_data(data_array, read_data_float, 0);
    }
    verb_print(HIGH, "DEBUG | unmap memory to fd and close file descriptor\n");
    munmap(ptr, sysconf(_SC_PAGESIZE));
    close(fd);
    return TRUE;
}