module fifo_axi_bridge (clk, rstn, s_axi_araddr, s_axi_arready, s_axi_arvalid, s_axi_awaddr,
                    s_axi_awready, s_axi_awvalid, s_axi_bready, s_axi_bresp, s_axi_bvalid,
                    s_axi_rdata, s_axi_rready, s_axi_rresp, s_axi_rvalid, s_axi_wdata, s_axi_wready,
                    s_axi_wstrb, s_axi_wvalid, fifo_data_in, fifo_data_in_vld, read_req);

// interface
input clk;
input rstn;
input [8:0] s_axi_araddr;
output s_axi_arready;
input s_axi_arvalid;
input [8:0] s_axi_awaddr;
output s_axi_awready;
input s_axi_awvalid;
input s_axi_bready;
output [1:0] s_axi_bresp;
output s_axi_bvalid;
output [31:0] s_axi_rdata;
input s_axi_rready;
output [1:0] s_axi_rresp;
output s_axi_rvalid;
input [31:0] s_axi_wdata;
output s_axi_wready;
input [3:0] s_axi_wstrb;
input s_axi_wvalid;

input [31:0] fifo_data_in;
input fifo_data_in_vld;
output read_req;

// regs - read machine
reg busy;
reg read_req_p;
reg rdata_valid;

// implement
always @(posedge clk) begin
    if (~rstn) begin
        busy = 1'b0;
        read_req_p = 1'b0;
        rdata_valid = 1'b0;
    end
    else begin
        if (~busy) begin
            busy = s_axi_arvalid;
            read_req_p = (s_axi_arvalid) ? 1'b1 : 1'b0;
        end
        else begin
            read_req_p = 1'b0;
            if (s_axi_rready) begin
                rdata_valid = 1'b1;
            end
            else begin
                rdata_valid = 1'b0;
                busy = 1'b0;
            end
        end
    end
end
//assign s_axi_arready = 1'b1;
assign s_axi_arready = busy;
assign read_req = read_req_p;
assign s_axi_rdata = fifo_data_in;
assign s_axi_rvalid = fifo_data_in_vld & rdata_valid;

endmodule
