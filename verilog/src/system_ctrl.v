
`define DOWN_SAMPLER

// file:        system_ctrl.v
// owner:       shaharf
// description: state machine to keep track of interaction between fw/logic.
// comments:    switch localparam to parameter according to vivdo support
//              add integer before paramter--> vivado compiler iterperate it as string

module system_ctrl(clk, rstn, clken, start_op, finish_op, event_start_op_when_system_not_ready,
                   event_finihs_op_when_system_not_ready, restart_vld, restart_type, 
                   event_restart_vld_when_system_not_in_finish_mode, start_config, phase_inc, 
                   event_start_config_when_state_is_not_idle, in_data_ch0, in_data_ch1, in_data_vld_ch0, in_data_vld_ch1, 
                   event_in_data_when_system_not_ready, out_data_ch0, out_data_ch1, out_data_vld_ch0, out_data_vld_ch1, 
                   out_addr_ch0, out_addr_ch1, data_count_ch0, data_count_ch1, phase_inc_vld);
//parameters
parameter FIFO_SIZE = 1024;
parameter integer FIFO_SIZE_WIDTH  = $clog2(FIFO_SIZE) + 1;

parameter DATA_WIDTH = 32;

parameter PHASE_INC_WIDTH = 16;

//system state
parameter IDLE = 0;
parameter CONFIG = 1;
parameter WAIT_FOR_START = 2;
parameter EXE = 3;
parameter FINISH = 4;
parameter NUM_OF_STATES = 5;
parameter integer NUM_OF_STATES_WIDTH = $clog2(NUM_OF_STATES);

//restart type
parameter REDO = 0;
parameter RECONFIG = 1;
parameter CLOSE = 2;
parameter NUM_OF_RESTART_TYPE = 3;
parameter integer NUM_OF_RESTART_TYPE_WIDTH = $clog2(NUM_OF_RESTART_TYPE);


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
input [PHASE_INC_WIDTH-1 : 0] phase_inc; 
output phase_inc_vld;
output event_start_config_when_state_is_not_idle;

// ---------------
// -- ctrl flow --
// ---------------
input [DATA_WIDTH-1 : 0] in_data_ch0; 
input in_data_vld_ch0;
input [DATA_WIDTH-1 : 0] in_data_ch1;
input in_data_vld_ch1;
output event_in_data_when_system_not_ready;

output [DATA_WIDTH-1 : 0] out_data_ch0;
output out_data_vld_ch0;
output [DATA_WIDTH-1 : 0] out_data_ch1;
output out_data_vld_ch1;
output [FIFO_SIZE_WIDTH -2 : 0] out_addr_ch0;
output [FIFO_SIZE_WIDTH -2 : 0] out_addr_ch1;
output [FIFO_SIZE_WIDTH - 1:0] data_count_ch0;
output [FIFO_SIZE_WIDTH - 1:0] data_count_ch1;

//implement

//state machine
reg [NUM_OF_STATES_WIDTH-1 : 0] state;
reg [NUM_OF_STATES_WIDTH-1 : 0] state_ns;
reg [1:0] delay_counter;
wire config_done = (delay_counter == 2'b11);

//misc
reg [FIFO_SIZE_WIDTH-1 : 0] fifo_size_ch0;
reg [FIFO_SIZE_WIDTH-1 : 0] fifo_size_ch1;
reg [FIFO_SIZE_WIDTH -2 :0] out_addr_p_ch0;
reg [FIFO_SIZE_WIDTH -2 :0] out_addr_p_ch1;
wire fifo_not_full_ch0 = fifo_size_ch0 < FIFO_SIZE;
wire fifo_not_full_ch1 = fifo_size_ch1 < FIFO_SIZE;
wire fifo_full_ch0 = fifo_size_ch0 == FIFO_SIZE;
wire fifo_full_ch1 = fifo_size_ch1 == FIFO_SIZE;
wire fifo_overflow_ch0 = fifo_size_ch0 > FIFO_SIZE;
wire fifo_overflow_ch1 = fifo_size_ch1 > FIFO_SIZE;

input [DATA_WIDTH-1 : 0] in_data_ch0_d; 
input in_data_vld_ch0_d;
input [DATA_WIDTH-1 : 0] in_data_ch1_d; 
input in_data_vld_ch1_d;

// shift reg section
`ifdef DOWN_SAMPLER
    shift_reg shift_reg_ch0(
        .clk(clk),
        .rstn(rstn),
        .data_in(in_data_ch0),
        .data_in_vld(in_data_vld_ch0),
        .data_out(in_data_ch0_d),
        .data_out_vld(in_data_vld_ch0_d)
        );

        shift_reg shift_reg_ch1(
            .clk(clk),
            .rstn(rstn),
            .data_in(in_data_ch1),
            .data_in_vld(in_data_vld_ch1),
            .data_out(in_data_ch1_d),
            .data_out_vld(in_data_vld_ch1_d)
        );
`else
    assign in_data_ch0_d = in_data_ch0;
    assign in_data_vld_ch0_d = in_data_vld_ch0;
    assign in_data_ch1_d = in_data_ch1;
    assign in_data_vld_ch1_d = in_data_vld_ch1;
`endif 
//

always @(posedge clk) begin
    if (~rstn) begin
        state <= {NUM_OF_STATES_WIDTH{1'b0}};
        delay_counter <= 2'b00;
        fifo_size_ch0 <= {FIFO_SIZE_WIDTH{1'b0}};
        fifo_size_ch1 <= {FIFO_SIZE_WIDTH{1'b0}};
        out_addr_p_ch0 <= {FIFO_SIZE_WIDTH-1{1'b0}};
        out_addr_p_ch1 <= {FIFO_SIZE_WIDTH-1{1'b0}};
    end
    else begin
        state <= state_ns;
        delay_counter <= (state == CONFIG) ? delay_counter + 2'b01 : 2'b00; 
        fifo_size_ch0 <= in_data_vld_ch0_d & fifo_not_full_ch0 ? fifo_size_ch0 + {{FIFO_SIZE_WIDTH-1{1'b0}},1'b1} :  
                     (state == IDLE) || (state == WAIT_FOR_START) ? {FIFO_SIZE_WIDTH{1'b0}} : fifo_size_ch0;

        fifo_size_ch1 <= in_data_vld_ch1_d & fifo_not_full_ch1 ? fifo_size_ch1 + {{FIFO_SIZE_WIDTH-1{1'b0}},1'b1} :  
                     (state == IDLE) || (state == WAIT_FOR_START) ? {FIFO_SIZE_WIDTH{1'b0}} : fifo_size_ch0;

        out_addr_p_ch0 <= in_data_vld_ch0_d & fifo_not_full_ch0 ? out_addr_p_ch0 + {{FIFO_SIZE_WIDTH-2{1'b0}},1'b1} :  
                     (state == IDLE) || (state == WAIT_FOR_START) ? {FIFO_SIZE_WIDTH-1{1'b0}} : out_addr_p_ch0;

        out_addr_p_ch1 <= in_data_vld_ch1_d & fifo_not_full_ch1 ? out_addr_p_ch1 + {{FIFO_SIZE_WIDTH-2{1'b0}},1'b1} :  
                     (state == IDLE) || (state == WAIT_FOR_START) ? {FIFO_SIZE_WIDTH-1{1'b0}} : out_addr_p_ch1;
    end
end

always @(*) begin
    case (state)
        IDLE: state_ns =  start_config ? CONFIG : IDLE;
        CONFIG: state_ns = config_done ? WAIT_FOR_START : CONFIG;
        WAIT_FOR_START: state_ns = start_op ? EXE : WAIT_FOR_START ;
        EXE: state_ns = fifo_full_ch0 & fifo_full_ch1 ? FINISH : EXE; 
        //todo shahar review syntax below
        FINISH: state_ns = restart_vld ? 
                        (restart_type == REDO) ? WAIT_FOR_START : 
                        (restart_type == RECONFIG) ? IDLE :
                        (restart_type == CLOSE) ? IDLE : FINISH :
                        FINISH;
        default: state_ns = IDLE; //todo shahar if we reach here unvalid state
    endcase
end



// always @(*posedge clk) begin
//     if (~rstn) begin
//         fifo_size <= {FIFO_SIZE_WIDTH{1'b0}};
//     end
//     else begin
//         fifo_size <= in_data_vld & fifo_not_full ? fifo_size + {{FIFO_SIZE_WIDTH-1{1'b0}},1'b1} : fifo_size;
//     end
// end

assign out_data_ch0 = in_data_ch0_d;
assign out_data_ch1 = in_data_ch1_d;
assign out_data_vld_ch0 = in_data_vld_ch0_d & fifo_not_full_ch0;
assign out_data_vld_ch1 = in_data_vld_ch1_d & fifo_not_full_ch1;
assign finish_op = (state == FINISH);
assign clken = (state == EXE);
assign out_addr_ch0 = out_addr_p_ch0;
assign out_addr_ch1 = out_addr_p_ch1;
assign data_count_ch0 = fifo_size_ch0;
assign data_count_ch1 = fifo_size_ch1;
assign phase_inc_vld = (state == CONFIG);
endmodule
