#ifndef _READ_WRITE_MOD
#define _READ_WRITE_MOD

#include <stdint.h>
#include "fw_data_struct.h"

//todo try to switch and user memory mapping to logic address spaces. insted of os call 
int write_to_logic(uint32_t data, uint64_t address); //write to logic and check it written. using os call. assuming before calling data converted to int (4B binary rep.) //todo implemenet 
uint32_t read_from_logic(uint64_t address); //read from logic. return data in int (binary rep)
int read_from_array(DataArray* data_array, int Ch);
int read_from_array_ch1(DataArray* data_array, int Ch);

#endif