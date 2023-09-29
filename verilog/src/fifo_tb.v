
module fifo_tb();

//parameter
parameter DATA_WIDTH = 32; 
parameter FIFO_SIZE = 1024;
localparam FIFO_SIZE_WIDTH = $clog2(FIFO_SIZE);

//regs - stub
reg clk, rstn;
reg [DATA_WIDTH-1 : 0] in_data;
reg in_data_vld;
reg out_data_vld;

//wire - output
wire out_data_rdy;
wire [DATA_WIDTH-1 : 0] out_data;
wire [FIFO_SIZE_WIDTH-1 : 0] out_data_ptr;
wire [FIFO_SIZE_WIDTH-1 : 0] fifo_size;
wire event_overflow;

//modules
fifo fifo_0(
    .clk(clk), 
    .rstn(rstn), 
    .in_data(in_data), 
    .in_data_vld(in_data_vld), 
    .out_data_rdy(out_data_rdy), 
    .out_data_vld(out_data_vld), 
    .out_data(out_data), 
    .out_data_ptr(out_data_ptr), 
    .fifo_size(fifo_size), 
    .event_overflow(event_overflow)
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
    in_data_vld = 1'b1;
    in_data = 32'h00000001;
    #10;
    in_data = 32'h00000002;
    #10;
    in_data_vld = 1'b0;
    #50;
    $display("[%0t] end simulation", $time);
    $finish;
end

always #5 clk = ~clk;

endmodule