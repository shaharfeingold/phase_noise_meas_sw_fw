#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "read_write.h"
#include "fw_data_struct.h"
#include "utils_function.h"
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "logic_config.h"
#include "error_handling.h"
// file:        logic_config.c
// owner:       shahar
// description: module to handle the config routine of the logic
// comment:     

void config_logic(LogicConfig* logic_config){
    uint32_t data_to_write = 0x00000000;
    verb_print(HIGH, "DEBUG | enterd config_logic\n");
    //write to logic start_config
    verb_print(MED, "Tracker | start config logic. send phase inc\n");
    //todo shahar add phase inc write after having support in logic side 
    verb_print(MED, "Tracker | enable logic to start config phase_inc\n");
    check_config_strcut(logic_config, "config");
    data_to_write = prepare_data_to_write(logic_config, TRUE, FALSE, FALSE);
    
    if (!write_to_logic(data_to_write, CONIG_BASE_ADDR)) {
        handle_medium_error("Failed to write config logic data");
        // Decide if the function should return or continue
    write_to_logic(data_to_write, CONIG_BASE_ADDR);
}

void config_start(LogicConfig* logic_config){
    uint32_t data_to_write = 0x00000000;
    verb_print(HIGH, "DEBUG | enterd config_start\n");
    //write to logic start_config
    //todo shahar add phase inc write after having support in logic side 
    verb_print(MED, "Tracker | enable logic to start operation\n");
    check_config_strcut(logic_config, "start");
    data_to_write = prepare_data_to_write(logic_config, FALSE, TRUE, FALSE);
    write_to_logic(data_to_write, CONIG_BASE_ADDR);
}

void wait_4_finish(LogicConfig* logic_config){
    verb_print(HIGH, "DEBUG | entered wait_4_finish\n");
    int finish_op = 0x00000000;
    while (TRUE){
        finish_op = (read_from_logic(FINISH_OP_DATA_COUNT_BASE_ADDR) & FINISH_OP_MASK) >> 11 ;
        verb_print(HIGH, "DEBUG | read_design, finish_op = %d\n", finish_op);
        sleep(2);
        if (finish_op == TRUE){
            return;
        }
    }
}

uint32_t prepare_restart_type_to_write(uint32_t end_type_byte){
    uint32_t result = 0x00000000;
    verb_print(HIGH, "DEBUG | enterd prepare_data_to_write with end_type_byte %x\n", end_type_byte);
    switch (end_type_byte)
    {
    case RESTART:{
        return (0x00000001 >> RESTART_TYPE_BIT);
        break;
    }
    case REDO:{
        return (0x00000000);
        break;
    }
    case END_CONNECTION:{
        return (0x00000002 >> RESTART_TYPE_BIT);
        break;
    }
    default:
        break;
    }
}

void send_restart_op_to_logic(LogicConfig* logic_config, uint32_t end_type_byte){
    verb_print(HIGH, "DEBUG | send_restart_op_to_logic\n");
    uint32_t data_to_write = 0x00000000;
    verb_print(MED, "Tracker | write restart type\n");
    data_to_write = prepare_restart_type_to_write(end_type_byte);
    write_to_logic(data_to_write, CONIG_BASE_ADDR);
    verb_print(MED, "Tracker | enable restart vld\n");
    data_to_write = prepare_data_to_write(logic_config, FALSE, FALSE, TRUE);
    write_to_logic(data_to_write, CONIG_BASE_ADDR);
}

uint32_t prepare_data_to_write(LogicConfig* logic_config, int start_config, int start_op, int restart_vld){
    verb_print(HIGH, "DEBUG | enterd prepare_data_to_write with start_config %d start_op = %d restart_vld = %d\n", start_config, start_op, restart_vld);
    uint32_t result = 0x00000000;
    if (start_config == TRUE && start_op == TRUE && restart_vld == TRUE){
        verb_print(MED, "in valid args passed\n");
        //todo shahar error handleing //todo shahar define routine
    }
    if (start_config == TRUE){
        result = 0x00000001 << (START_CONFIG_BIT);
    }
    if (start_op == TRUE){
        result = 0x00000001 << (START_OP_BIT);
    }
    if (restart_vld == TRUE){
        result = 0x00000001 << (RESTART_VLD_BIT);
    }

    verb_print(HIGH, "DEBUG | return value from prepare_data_to_write = %d\n", result);
    return result;
}

void check_config_strcut(LogicConfig* logic_config, char* state){
    verb_print(MED, "DEBUG | entered check_config_strcut\n");
    int start_config = logic_config->config_sent;
    uint32_t phase_inc = logic_config->phase_inc;
    //int restart_vld = //todo shahar add to logic_config restart fields: vld & type
    int start_op = logic_config->start_sent;
    int result = TRUE;

    if (strcmp(state, "config")){
        if (start_config == 0 || phase_inc == 0)
            result = FALSE;
    }

    if (strcmp(state, "start")){
        if (start_op == 0)
            result = FALSE;
    }

    if (strcmp(state, "restart")){
        //todo shahar contiune
    }

    verb_print(MED, "DEBUG | check result = %s\n", result == 1 ? "TRUE" : "FALSE");
    //todo shahar need to define routine how to handle

     if (result == FALSE) {
        handle_easy_error("Configuration structure validation failed in check_config_strcut");
        // decide how to proceed after this error
    }
}