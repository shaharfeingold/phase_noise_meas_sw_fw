

module load_ctrl_tb();
//parameter
parameter BASE_ADDR = 64'h0000000000000000; //todo shahar switch to the right addr.
parameter ADDR_WIDTH = 64;
parameter DATA_WIDTH = 4;
parameter FIFO_SIZE = 5;

//stubs
reg clk;
reg rstn;

reg request_vld;
reg[ADDR_WIDTH-1 : 0] addr; //todo shahar to use for a senity check

reg [DATA_WIDTH-1 : 0] data_in;
reg data_in_rdy;

//outputs
output event_current_data_to_be_read_is_not_in_order_with_given_addr;
output data_in_vld; //when this bit is set we are issuing a read req
output event_read_req_when_no_data_is_available;
output [DATA_WIDTH-1 : 0] data_out;
output data_out_vld;

//instance  of top module
load_ctrl #(.DATA_WIDTH(DATA_WIDTH), .FIFO_SIZE(FIFO_SIZE))load_ctrl_0 (
    .clk(clk), 
    .rstn(rstn), 
    .request_vld(request_vld), 
    .addr(addr), 
    .event_current_data_to_be_read_is_not_in_order_with_given_addr(event_current_data_to_be_read_is_not_in_order_with_given_addr),
    .data_in(data_in), 
    .data_in_rdy(data_in_rdy), 
    .data_in_vld(data_in_vld), 
    .event_read_req_when_no_data_is_available(event_read_req_when_no_data_is_available),
    .data_out(data_out), 
    .data_out_vld(data_out_vld)
    );


//sim flow
initial begin
    $display("[%0t] stat simulation", $time);
    $display("[%0t] init vcd file", $time);
    $dumpfile("output.vcd"); // Specify the VCD file name
    $dumpvars(0, load_ctrl_tb); // Replace "top_module" with the top-level module instance name
    // for (i = 0; i < FIFO_SIZE; i = i + 1) begin //todo shahar an attempt to dump the data itself inside the fifo
    //     $dumpvars(0, fifo_tb.fifo_0.data[i]);
    // end
    $display("[%0t] finish open vcd file", $time);
    clk = 1'b0;
    rstn = 1'b1;
    request_vld = 1'b0;
    data_in = 4'h1;
    data_in_rdy = 1'b0;
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
    data_in_rdy = 1'b1;
    #10;
    request_vld = 1'b1;
    #10;
    // request_vld = 1'b0;
    data_in = 4'h2;
    #10;
    data_in = 4'h3;
    #10;
    data_in = 4'h4;
    #10;
    request_vld = 1'b0;
    #50;
    $display("[%0t] end simulation", $time);
    $finish;
end

always #5 clk = ~clk;

endmodule