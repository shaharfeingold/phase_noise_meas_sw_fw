`timescale 1ns / 1ps
//`include "async_fifo.v"

module async_fifo_tb ();

    parameter DATA_WIDTH = 32;
    parameter FIFO_DEPTH = 3;
    parameter FIFO_DEPTH_WIDTH = $clog2(FIFO_DEPTH);

    //ports
    reg rst; //generate a rest for each domain with it clock speratly
    // read i/f
    reg rd_clk;
    reg read_req;
    wire data_out_vld;
    wire [DATA_WIDTH-1 : 0] data_out;
    wire fifo_empty;

    //write i/f
    reg wr_clk;
    reg data_in_vld;
    reg [DATA_WIDTH-1 : 0] data_in;
    wire fifo_full;

async_fifo #(.DATA_WIDTH(DATA_WIDTH), .FIFO_DEPTH(FIFO_DEPTH)) top (
    .rst(rst),
    .rd_clk(rd_clk),
    .read_req(read_req),
    .data_out_vld(data_out_vld),
    .data_out(data_out),
    .fifo_empty(fifo_empty),
    .wr_clk(wr_clk),
    .data_in_vld(data_in_vld),
    .data_in(data_in),
    .fifo_full(fifo_full)
);

reg start_clk1;
reg start_clk2;

initial begin
    $dumpfile("async_fifo_tb_dump.vcd");
    $dumpvars(0, async_fifo_tb, async_fifo_tb.top.memory[0], async_fifo_tb.top.memory[1], async_fifo_tb.top.memory[2]);
rd_clk = 1'b0;
wr_clk = 1'b0;
rst = 1'b0;
data_in_vld = 1'b0;
read_req = 1'b0;
start_clk1 = 1'b1;
start_clk2 = 1'b0;
$display("start simulation\n");
#1;
$display("start reset\n");
rst = 1'b1;  
#100;
rst = 1'b0;
$display("finish reset\n");
#10;
$display("!! push data and read\n");
data_in_vld = 1'b1;
read_req = 1'b1;
data_in = 1;
#10;
data_in = 2;
#10;
data_in = 3;
#10;
data_in = 4;
#10;
data_in_vld = 1'b1;
#1000;
$display("going to leave simulation");
$finish;
end


always begin
    if (start_clk1) begin
        #5 wr_clk = ~wr_clk;
    end
end

always begin
    if (start_clk2) begin
        #10 rd_clk = ~rd_clk;
    end
    else begin
        start_clk2 = 1'b1;
        #13;
    end
end

// always begin
//     if (~start_clk2) begin
//         #13;
//         $display("enabling clk_2");
//         start_clk2 = 1'b1;
//     end
// end


endmodule