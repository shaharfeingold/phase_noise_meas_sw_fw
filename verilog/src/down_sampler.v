`timescale 1ns/1ps

// file: 
// owner:
// description:

module down_sampler (clk, rstn, data_in, data_in_vld, data_out, data_out_vld);
    // parameters
    parameter DATA_WIDTH = 32;
    parameter DOWN_SAMPLER_BY = 10;
    parameter integer DOWN_SAMPLER_WIDTH = $clog2(DOWN_SAMPLER_BY);
    
    // interfaces
    input clk;
    input rstn;
    input [DATA_WIDTH-1 : 0] data_in;
    input data_in_vld;
    output [DATA_WIDTH-1 : 0] data_out;
    output data_out_vld;

    // implemantaion
    reg [DOWN_SAMPLER_WIDTH-1 : 0] vld_counter;
    wire max_count = vld_counter == 4'd9; //todo review hardcoded
    wire count_init_value_d = vld_counter == {DOWN_SAMPLER_WIDTH{1'b0}};
    reg count_init_value;

    reg [DATA_WIDTH-1 : 0] data_in_d;
    reg data_in_vld_d;
    
    always @(posedge clk ) begin
        if (~rstn) begin
            vld_counter <= {DOWN_SAMPLER_WIDTH{1'b0}};
            count_init_value <= 1'b0;
            data_in_d <= {DATA_WIDTH{1'b0}};
            data_in_vld_d <= 1'b0;
        end
        else begin
            vld_counter <= max_count ? {DOWN_SAMPLER_WIDTH{1'b0}} : 
                                        vld_counter + {{DOWN_SAMPLER_WIDTH-1{1'b0}},data_in_vld};
            count_init_value <= count_init_value_d;
            data_in_d <= data_in;
            data_in_vld_d <= data_in_vld;
        end
    end
    // shift_reg shift_reg_0 (
    //     .clk(clk),
    //     .rstn(rstn),
    //     .data_in(data_in),
    //     .data_in_vld(data_in_vld),
    //     .data_out(data_out),
    //     .data_out_vld(data_out_vld)
    // );

    assign data_out = data_in_d;
    assign data_out_vld = data_in_vld_d & (count_init_value);

endmodule
