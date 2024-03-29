#ifndef _UTILS_FUNC_LIB
#define _UTILS_FUNC_LIB

#define ADDRESS_WIDTH 64
#define ADDRESS_WIDTH_HEX 8+2+1 //8 hex symbol + 2 leading chars + null char
#define TRUE 1 // todo move to other file // general defines
#define FALSE 0 // todo move to other file // general defines
#define HIGH 3
#define MED 2
#define NONE 1
#define OFF 0

#define VERB HIGH //right know verb for all the module (shared verbose) //todo shahar change to off when finish

#include <stdint.h>

void convert_hex_to_string(uint64_t address, char target_string[]);
void convert_hex_to_string_unformnatted(uint64_t num, char target_string[]);
void verb_print(int verbose, const char* format, ...);
uint32_t convert_string_to_hex(char src_string[]);
uint64_t convert_string_to_hex_uint64_t(char src_string[]);
uint32_t system_call(char cmd[]); //todo create a generic function to create a system call with a pipe.

uint32_t encode_uint_data_to_send(float data_as_uint);
void build_data_packet_header(char* header, uint32_t len, int type);

float convert_fix_point_to_float(uint32_t fixed_point, int Ch);
#endif
