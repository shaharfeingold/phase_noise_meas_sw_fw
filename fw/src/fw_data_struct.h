#ifndef _FW_DATA_STRUCT
#define _FW_DATA_STRUCT

#include <stdint.h>

//struct declaration
typedef struct _LogicConfig{
    uint32_t phase_inc;
    uint32_t config_sent;
    uint32_t start_sent;
    uint32_t got_finish;
}LogicConfig;

typedef struct _Events{
    uint32_t EventsBitsVector;
    //todo shahar add evenets signals according to logic designs
}Events;

//struct functions:
void init_logic_config_struct(LogicConfig* new_logic_config);
void UpdateConfigSent(LogicConfig* logic_config, uint32_t data);
void UpdateConfigSent(LogicConfig* logic_config, uint32_t data);
void UpdateConfigSent(LogicConfig* logic_config, uint32_t data);
void UpdateConfigSent(LogicConfig* logic_config, uint32_t data);

void init_events_struct(Events* new_event_struct);

#endif