#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "read_write.h"
#include "utils_function.h"

int main(){
    uint64_t address = 0x42000000;
    verb_print(MED, "starting simulation\n");
    verb_print(MED, "trying to read memory %x\n",address);
    uint32_t read_data = read_from_logic(address);
    verb_print(MED, "read data = %d\n", read_data);
    verb_print(MED, "finish simulation, leaving now\n");

    return 0;
}