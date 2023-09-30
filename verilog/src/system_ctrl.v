

module system_ctrl();
//parameters
parameter FIFO_SIZE = 1024;
localparam FIFO_SIZE_WIDTH  = $clog2(FIFO_SIZE);

parameter DATA_WIDTH = 32;

//system state
localparam IDLE = 0;
localparam CONFIG = 1;
localparam WAIT_FOR_START = 2;
localparam EXE = 3;
localparam FINISH = 4;
localparam NUM_OF_STATES = 5;
localparam NUM_OF_STATES_WIDTH = $clog2(NUM_OF_STATES);

//restart type
localparam REDO = 0;
localparam RECONFIG = 1;
localparam CLOSE = 2;
localparam NUM_OF_RESTART_TYPE = 3;
localparam NUM_OF_RESTART_TYPE_WIDTH = $clog2(NUM_OF_RESTART_TYPE);


//interfaces
//  --------------
//  --- general---
// ---------------
input clk;
input rstn;
output clken; // clk en to drive all units to start measuring 

// ----------------
// -- ctrl to fw --
// ----------------
input start_op;
output finish_op;
output event_start_op_when_system_not_ready; //todo connect
output event_finihs_op_when_system_not_ready; //todo connect

input restart_vld;
input [NUM_OF_RESTART_TYPE_WIDTH-1 : 0] restart_type;
output event_restart_vld_when_system_not_in_finish_mode;

//ouside_interface:
//start_confg | phase_inc
input start_config;
input [DATA_WIDTH-1 : 0] phase_inc; 
output start_config_when_state_is_not_idle;

// ---------------
// -- ctrl flow --
// ---------------
input [DATA_WIDTH-1 : 0] in_data;
input in_data_vld;
output event_in_data_when_system_not_ready;

output [DATA_WIDTH-1 : 0] out_data;
output out_data_vld;

//implement

//state machine
reg [NUM_OF_STATES_WIDTH-1 : 0] state;
reg [NUM_OF_STATES_WIDTH-1 : 0] state_ns;
reg [1:0] delay_counter;
wire config_done = (delay_counter == 2'b10);

always @(posedge clk) begin
    if (~rstn) begin
        state <= {NUM_OF_STATES_WIDTH{1'b0}};
        delay_counter <= 2'b00;
    end
    else begin
        state <= state_ns;
        delay_counter <= (state == CONFIG) ? delay_counter + 2'b01 : 2'b00; 
    end
end

case (state)
    IDLE: state_ns =  start_config ? CONFIG : IDLE;
    CONFIG: state_ns = config_done ? WAIT_FOR_START : CONFIG;
    WAIT_FOR_START: state_ns = start_op ? EXE : WAIT_FOR_START ;
    EXE: state_ns = fifo_full ? FINISH : EXE; 
    FINISH: state_ns = restart_vld ?                                //todo shahar review this syntax
                       (restart_type == REDO) ? WAIT_FOR_START : 
                       (restart_type == RECONFIG) ? IDLE :
                       (restart_type == CLOSE) ? IDLE :
                       FINISH;

    default: state_ns = IDLE; //todo shahar if we reach here unvalid state
endcase

//misc
reg [FIFO_SIZE_WIDTH-1 : 0] fifo_size;
wire fifo_not_full = fifo_size < FIFO_SIZE;
wire fifo_full = fifo_size == FIFO_SIZE;
wire fifo_overflow = fifo_size > FIFO_SIZE;

always @(*posedge clk) begin
    if (~rstn) begin
        fifo_size <= {FIFO_SIZE_WIDTH{1'b0}};
    end
    else begin
        fifo_size <= in_data_vld & fifo_not_full ? fifo_size + {FIFO_SIZE_WIDTH-1{1'b0},1'b1} : fifo_size;
    end
end

assign out_data = in_data;
assign out_data_vld = in_data & fifo_not_full;
assign finish_op = (state == FINISH);
assign clken = (state == EXE);
endmodule