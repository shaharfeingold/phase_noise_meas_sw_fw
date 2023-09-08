#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "read_write.h"
#include "utils_function.h"

int main(){
    verb_print(MED, "starting simulation\n");
    verb_print(MED, "trying to read memory 0x42000000\n");
    uint32_t read_data = read_from_logic(0x42000000);
    verb_print(MED, "read data = %d\n", read_data);
    verb_print(MED, "finish simulation, leaving now\n");

    return 0;
}