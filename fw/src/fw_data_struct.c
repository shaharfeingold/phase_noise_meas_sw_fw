#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "utils_function.h"
#include "fw_data_struct.h"

// file:        fw_data_struct.c
// owner:       shahar
// description: data structs for the fw and their corosponding functions.
// comment:     


//strcut function
//get a pointer for a new allocated strct and modify it inside.
//modify all field to zero.
void init_logic_config_struct(LogicConfig* new_logic_config){
    new_logic_config->config_sent = 0;
    new_logic_config->got_finish = 0;
    new_logic_config->phase_inc = 0;
    new_logic_config->start_sent = 0;
}

//update field config_sent.
void UpdateConfigSent(LogicConfig* logic_config, uint32_t data){
    logic_config->config_sent = data;
}

//update field got_finish.
void UpdateConfigSent(LogicConfig* logic_config, uint32_t data){
    logic_config->got_finish = data;
}

//update field phase_inc.
void UpdateConfigSent(LogicConfig* logic_config, uint32_t data){
    logic_config->phase_inc = data;
}

//update field start_sent.
void UpdateConfigSent(LogicConfig* logic_config, uint32_t data){
    logic_config->start_sent = data;
}

//get a pointer for a new allocated strct and modify it inside.
//modify all field to zero.
void init_events_struct(Events* new_event_struct){
    new_event_struct->EventsBitsVector = 0x0000;
    //todo shahar init more fields upon their addition.
}

//todo shahar make function to set bit event.