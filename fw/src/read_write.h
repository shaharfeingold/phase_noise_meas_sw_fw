#ifndef _READ_WRITE_MOD
#define _READ_WRITE_MOD

//todo try to switch and user memory mapping to logic address spaces. insted of os call 
int write_to_logic(uint32_t data, uint64_t address); //write to logic and check it written. using os call. assuming before calling data converted to int (4B binary rep.) //todo implemenet 
uint32_t read_from_logic(uint32_t address); //read from logic. return data in int (binary rep)

#endif