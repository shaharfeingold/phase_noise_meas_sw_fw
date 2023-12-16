#ifndef _LOGIC_CONFIG_MOD
#define _LOGIC_CONFIG_MOD

#include <stdint.h>
#include "fw_data_struct.h"

void config_logic(LogicConfig* logic_config);
uint32_t prepare_data_to_write(LogicConfig* logic_config, int start_config, int start_op, int restart_vld);

#endif