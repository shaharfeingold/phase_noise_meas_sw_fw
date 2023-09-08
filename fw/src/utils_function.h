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

#define VERB OFF //right know verb for all the module (shared verbose)

#include <stdint.h>

void convert_hex_to_string(uint64_t address, char target_string[]);
void verb_print(int verbose, const char* format, ...);

#endif