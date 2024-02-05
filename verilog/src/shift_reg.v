`timescale 1ns/1ps
//`include "ff.v"

module shift_reg (clk, rstn, data_in, data_in_vld, data_out, data_out_vld);
    // parameters
    parameter DATA_WIDTH = 32;
    parameter SHIFT_REG_WIDTH = 10;
    
    // interfaces
    input clk;
    input rstn;
    input [DATA_WIDTH-1 : 0] data_in;
    input data_in_vld;
    output [DATA_WIDTH-1 : 0] data_out;
    output data_out_vld;

    // implemantion
    wire [DATA_WIDTH-1 : 0] data_d [0 : SHIFT_REG_WIDTH-1];
    wire [SHIFT_REG_WIDTH-1:0] data_vld_d;
    wire [SHIFT_REG_WIDTH-1:0] clk_d = {SHIFT_REG_WIDTH{clk}};
    wire [SHIFT_REG_WIDTH-1:0] rstn_d = {SHIFT_REG_WIDTH{rstn}};

    generate
        genvar i;
        for (i= (SHIFT_REG_WIDTH-1) ; i > 0 ; i = i - 1) begin : gen_loop
            ff ff_0 (
                .clk(clk_d[i:i]),
                .rstn(rstn_d[i:i]),
                .data_ns(data_d[i]),
                .data_ns_vld(data_vld_d[i:i]),
                .data_ps(data_d[i-1]),
                .data_ps_vld(data_vld_d[i-1:i-1])
            );
        end
    endgenerate
assign data_d[SHIFT_REG_WIDTH-1] = data_in;
assign data_vld_d [SHIFT_REG_WIDTH-1:SHIFT_REG_WIDTH-1] = data_in_vld;
assign data_out = data_d[0];
assign data_out_vld = data_vld_d[0:0];
endmodule