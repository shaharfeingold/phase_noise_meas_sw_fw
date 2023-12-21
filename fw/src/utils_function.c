#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>
#include "utils_function.h"
#include "defines.h"
#include <arpa/inet.h>
#include "error_handling.h"
// file:        utils_function.c
// owner:       shahar
// description: lib with util , general purpose function.
// comment:     

void convert_hex_to_string(uint64_t address, char target_string[]){
    // Ensure target_string is large enough for the formatted string
    if (target_string == NULL) {
        handle_easy_error("Target string is NULL in convert_hex_to_string");
        return;
    }
    verb_print(HIGH, "enterd covert_hex_to_string with address = 0x%x\n", address);
    sprintf(target_string, "0x%016llx", address);
    verb_print(HIGH, "returning from covert_hex_to_string with target_sting = %s\n", target_string);
}

void convert_hex_to_string_unformnatted(uint64_t num, char target_string[]){
    verb_print(HIGH, "enterd covert_hex_to_string_unformatted with num = 0x%x\n", num);
    sprintf(target_string, "%016llx", num);
    verb_print(HIGH, "returning from covert_hex_to_string_unformatted with target_sting = %s\n", target_string);
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

uint32_t convert_string_to_hex(char src_string[]){
    return strtoul(src_string, NULL, 16);
}

uint64_t convert_string_to_hex_uint64_t(char src_string[]){
    return strtoull(src_string, NULL, 16);
}

uint32_t encode_uint_data_to_send(uint32_t data_as_uint){
    uint32_t result = htonl(data_as_uint);
    return result;
}

void build_data_packet_header(char* header, uint32_t len, int type){
    verb_print(HIGH, "entered build_data_packet_header with : len = %d, type = %d", len, type);
    //todo shahar document the packet header format in external doc 
    //packet header format:
    // 1B       | 4B    | 1B
    // pkt_type | len   | array_type
    uint64_t array_type_casted = 0;
    switch (type) {
    case 0:
        array_type_casted = 0x0000000000000000;
        break;
    case 1:
        array_type_casted = 0x0000000000010000;
        break;
    case 2:
        array_type_casted = 0x0000000000020000;
        break;
    default:
       handle_fatal_error("Invalid type in build_data_packet_header");
    }
    //uint64_t pkt_type_casted = DATA_PKT;
    uint64_t pkt_type_casted = 0x0200000000000000;
    uint64_t len_casted = (uint64_t)len << 24; //todo shahar review this line.
    uint64_t result = 0x0000000000000000;
    result = pkt_type_casted | len_casted | array_type_casted;
    sprintf(header, "%016llx", result);
    verb_print(HIGH, "the header result from the encoder = %s", header);
}