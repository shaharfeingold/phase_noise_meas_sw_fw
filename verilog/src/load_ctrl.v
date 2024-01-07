
module load_ctrl(clk, rstn, request_vld, addr, event_current_data_to_be_read_is_not_in_order_with_given_addr,
                 data_in, data_in_rdy, data_in_vld, event_read_req_when_no_data_is_available,
                 data_out, data_out_vld);
//parameters
parameter BASE_ADDR = 64'h0000000000000000; //todo shahar switch to the right addr.
parameter ADDR_WIDTH = 64;
parameter DATA_WIDTH = 32;
parameter FIFO_SIZE = 1024;
parameter integer  FIFO_SIZE_WIDTH = $clog2(FIFO_SIZE) + 1;

//interface
//addr request from design
input clk;
input rstn;

input request_vld;
input[ADDR_WIDTH-1 : 0] addr; //todo shahar to use for a senity check
output event_current_data_to_be_read_is_not_in_order_with_given_addr;

//data from fifo
input [DATA_WIDTH-1 : 0] data_in;
input data_in_rdy;
output data_in_vld; //when this bit is set we are issuing a read req
output event_read_req_when_no_data_is_available;

//data toward fw
output [DATA_WIDTH-1 : 0] data_out;
output data_out_vld;

//implementaion
reg [FIFO_SIZE_WIDTH-1 : 0] data_count_read;
reg data_out_vld_p;
reg [DATA_WIDTH-1 : 0] data_out_p;

always @(posedge clk ) begin
    if (~rstn) begin
        data_count_read <= {FIFO_SIZE_WIDTH{1'b0}};
        data_out_vld_p <= 1'b0;
        data_out_p = {DATA_WIDTH{1'b0}};
    end
    else begin
        data_count_read <= request_vld & data_in_rdy ? data_count_read + {{FIFO_SIZE_WIDTH-1{1'b0}},1'b1} : data_count_read;
        data_out_vld_p <= request_vld & data_in_rdy;
        data_out_p <= data_in;
    end
end

assign data_out = data_out_p;
assign data_out_vld = data_out_vld_p;
assign data_in_vld = request_vld;


endmodule
