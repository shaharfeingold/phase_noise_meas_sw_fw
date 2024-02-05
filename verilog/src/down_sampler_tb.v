`timescale 1ns/1ps

// file
// owner
// description

module down_sampler_tb ();
    // parameters
    parameter DATA_WIDTH = 32;
    reg clk;
    reg rstn;

    reg [DATA_WIDTH-1:0] data_in;
    reg data_in_vld;

    wire [DATA_WIDTH-1:0] data_out;
    wire data_out_vld;

    down_sampler top (
        .clk(clk),
        .rstn(rstn),
        .data_in(data_in),
        .data_in_vld(data_in_vld),
        .data_out(data_out),
        .data_out_vld(data_out_vld)
    );

    initial begin
        $display("START simulation");
        $dumpfile("down_sampler_tb_dump.vcd");
        $dumpvars(0);
        clk = 0;
        rstn = 0;
        data_in_vld = 1'b0;
        #100;
        rstn = 1;
        #10;
        #1;
        data_in = 32'h00001234;
        data_in_vld = 1'b1;
        #10;
        data_in_vld = 1'b0;
        #10000;
        $display("Simulation Finish");
        $finish;
    end

    always #5 clk = ~clk;

endmodule