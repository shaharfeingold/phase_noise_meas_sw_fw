#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "read_write.h"
#include "utils_function.h"
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

// file:        read_write.c
// owner:       shahar
// description: module to read and write to logic unit.
// comment:     currntly support write / read  through os call. later by template can switch to mem mapping
//              need to support also catch the return code from the read/write.

int write_to_logic(uint32_t data, uint64_t address){
    verb_print(MED, "DEBUG | enter write_to_logic with : data = %d, address = %x\n", data, address);
    // varaible
    int fd;
    void *ptr;
    char *name = "/dev/mem";
    uint32_t read_data = 0x0000;
    verb_print(HIGH, "DEBUG | open a fd to catch the read\n");
    if((fd = open(name, O_RDWR)) < 0)
        {
        perror("open");
        return 1;
        }
    verb_print(HIGH, "DEBUG | map the fd to memory address for the write and read");
    ptr = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, address); //each pointer of this map is width of 1B (memory is addrsable by bytes)
    // write data to logic
    verb_print(HIGH, "DEBUG | write data on addr 0x%x with the value %d\n", address, data);
    *((uint32_t *)(ptr)) = data;   
    verb_print(HIGH, "DEBUG | unmap memory to fd and close file descriptor\n");
    munmap(ptr, sysconf(_SC_PAGESIZE));
    close(fd);
    // read data from address.
    read_data = read_from_logic(address);
    if (read_data == data){
        verb_print(HIGH, "DEBUG | read_data equal to data asked to be written\n");
        return TRUE;
    }
    verb_print(HIGH, "DEBUG | read_data not equal to data asked to be written\n");
    return FALSE;
}

uint32_t read_from_logic(uint64_t address){
    verb_print(MED, "DEBUG | enterd read_from_logic with addr = %x\n", address);
    //variables
    int fd;
    void *ptr;
    char *name = "/dev/mem";
    uint32_t read_data = 0x0000;
    verb_print(HIGH, "DEBUG | open a fd to catch the read\n");
    if((fd = open(name, O_RDWR)) < 0)
        {
        perror("open");
        return 1;
        }
    verb_print(HIGH, "DEBUG | map the fd to memory address for the read");
    ptr = mmap(NULL, sysconf(_SC_PAGESIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, address); //each pointer of this map is width of 1B (memory is addrsable by bytes)
    read_data = *((uint32_t *)(ptr + 0)); //the output is in the base address of the memory mapping. 
    read_data = *((uint32_t *)(ptr + 0)); //the output is in the base address of the memory mapping.
    verb_print(HIGH, "DEBUG | Read data from logic = %x\n",read_data);
    verb_print(HIGH, "DEBUG | unmap memory to fd and close file descriptor\n");
    munmap(ptr, sysconf(_SC_PAGESIZE));
    close(fd);
    return read_data;

    
      

  





  

  
}