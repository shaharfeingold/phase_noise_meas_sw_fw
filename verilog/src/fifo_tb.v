
module fifo_tb();

//parameter
parameter DATA_WIDTH = 32; 
parameter FIFO_SIZE = 5;
localparam FIFO_SIZE_WIDTH = $clog2(FIFO_SIZE);

//regs - stub
reg clk, rstn;
reg [DATA_WIDTH-1 : 0] in_data;
reg in_data_vld;
reg out_data_vld;
reg flush_fifo;

//wire - output
wire out_data_rdy;
wire [DATA_WIDTH-1 : 0] out_data;
wire [FIFO_SIZE_WIDTH-1 : 0] out_data_ptr;
wire [FIFO_SIZE_WIDTH : 0] fifo_size;
wire event_overflow;
wire event_underrun;

//modules
fifo #(.FIFO_SIZE(5)) fifo_0(
    .clk(clk), 
    .rstn(rstn), 
    .in_data(in_data), 
    .in_data_vld(in_data_vld), 
    .out_data_rdy(out_data_rdy), 
    .out_data_vld(out_data_vld), 
    .out_data(out_data), 
    .out_data_ptr(out_data_ptr), 
    .fifo_size(fifo_size), 
    .flush_fifo(flush_fifo),
    .event_overflow(event_overflow),
    .event_underrun(event_underrun)
    );

//simulation
initial begin
    $display("[%0t] stat simulation", $time);
    $display("[%0t] init vcd file", $time);
    $dumpfile("output.vcd"); // Specify the VCD file name
    $dumpvars(0, fifo_tb); // Replace "top_module" with the top-level module instance name
    $display("[%0t] finish open vcd file", $time);
    clk = 1'b0;
    rstn = 1'b1;
    in_data_vld = 1'b0;
    out_data_vld = 1'b0;
    flush_fifo = 1'b0;
    $display("[%0t] start reset dut", $time);
    #1 
    rstn = 1'b0;
    #20;
    $display("[%0t] finish reset dut", $time);
    rstn = 1'b1;
    $display("[%0t]start relax", $time);
    #50;
    $display("[%0t] finish relax", $time);
    $display("[%0t] start inject data", $time);
    $display("[%0t] write the value 32'h00000001", $time);
    in_data_vld = 1'b1;
    in_data = 32'h00000001;
    #10;
    $display("[%0t] fifo size = %d", $time ,fifo_size);
    $display("[%0t] write the value 32'h00000002", $time);
    in_data = 32'h00000002;
    #10;
    $display("[%0t] fifo size = %d", $time ,fifo_size);
    in_data_vld = 1'b0;
    #10;
    //read and write at the same clock
    $display("[%0t] write the value 32'h00000003", $time);
    $display("[%0t] issue a read from the fifo", $time);
    out_data_vld = 1'b1;
    in_data_vld = 1'b1;
    in_data = 32'h00000003;
    #10;
    $display("[%0t] fifo size = %d", $time ,fifo_size);
    $display("[%0t] issue a read from the fifo", $time);
    in_data_vld = 1'b0;
    out_data_vld = 1'b1;
    #10;
    $display("[%0t] fifo size = %d", $time ,fifo_size);
    out_data_vld = 1'b0;
    //sim over flow 
    $display("[%0t] going to inject overflow", $time);
    #10;
    $display("[%0t] write the value 32'h00000004", $time);
    in_data_vld = 1'b1;
    in_data = 32'h00000004;
    #10;
    $display("[%0t] fifo size = %d", $time ,fifo_size);
    $display("[%0t] write the value 32'h00000005", $time);
    in_data = 32'h00000005;
    #10; 
    $display("[%0t] fifo size = %d", $time ,fifo_size);
    $display("[%0t] write the value 32'h00000006", $time);
    in_data = 32'h00000006;
    #10; 
    $display("[%0t] fifo size = %d", $time ,fifo_size);
    $display("[%0t] write the value 32'h00000007", $time);
    in_data = 32'h00000007;
    #10;
    $display("[%0t] fifo size = %d", $time ,fifo_size);
    $display("[%0t] write the value 32'h00000008", $time);
    in_data = 32'h00000008;
    #10;
    $display("[%0t] fifo size = %d", $time ,fifo_size);
    $display("[%0t] overflow ? %d", $time, event_overflow);
    in_data_vld = 1'b0;
    #50;
    //flush fifo
    $display("[%0t] flushing fifo", $time);
    flush_fifo = 1'b1;
    #10;
    flush_fifo = 1'b0;
    #50;
    $display("[%0t] end simulation", $time);
    $finish;
end

always #5 clk = ~clk;

endmodule