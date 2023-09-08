#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include "utils_function.h"

// file:        utils_function.c
// owner:       shahar
// description: lib with util , general purpose function.
// comment:     

void convert_hex_to_string(uint64_t address, char target_string[]){
    sprintf(target_string, "%08x");
}

void verb_print(int verbose, const char* format, ...) {
    if (verbose == 0){
        return;
    }
    if (verbose <= VERB) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}