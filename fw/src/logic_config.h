#ifndef _LOGIC_CONFIG_MOD
#define _LOGIC_CONFIG_MOD

#include <stdint.h>
#include "fw_data_struct.h"

void config_logic(LogicConfig* logic_config);
uint32_t prepare_data_to_write(LogicConfig* logic_config, int start_config, int start_op, int restart_vld);
void config_start(LogicConfig* logic_config);
void wait_4_finish(LogicConfig* logic_config, int NumOfChannels);
void check_config_strcut(LogicConfig* logic_config, char* state);
void send_restart_op_to_logic(LogicConfig* logic_config, uint32_t end_type_byte);
uint32_t prepare_restart_type_to_write(uint32_t end_type_byte);
uint32_t prepare_data_to_write_phase_inc(uint32_t phase_inc);

#endif