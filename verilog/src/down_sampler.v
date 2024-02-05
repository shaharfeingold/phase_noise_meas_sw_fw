`timescale 1ns/1ps

// file: 
// owner:
// description:

module down_sampler (clk, rstn, data_in, data_in_vld, data_out, data_out_vld);
    // parameters
    parameter DATA_WIDTH = 32;
    parameter DOWN_SAMPLER_BY = 10;
    
    // interfaces
    input clk;
    input rstn;
    input [DATA_WIDTH-1 : 0] data_in;
    input data_in_vld;
    output [DATA_WIDTH-1 : 0] data_out;
    output data_out_vld;

    // implemantaion
    
    shift_reg shift_reg_0 (
        .clk(clk),
        .rstn(rstn),
        .data_in(data_in),
        .data_in_vld(data_in_vld),
        .data_out(data_out),
        .data_out_vld(data_out_vld)
    );

endmodule
