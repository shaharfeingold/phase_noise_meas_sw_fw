
module ff(clk, rstn, data_ns, data_ns_vld, data_ps, data_ps_vld);
//parameters
parameter DATA_WIDTH = 32;

//ports
input clk, rstn;

input [DATA_WIDTH-1 : 0] data_ns;
input data_ns_vld;

output reg [DATA_WIDTH-1 : 0] data_ps;
output reg data_ps_vld;

//implemention
always @(posedge clk) begin
    if (~rstn) begin
        data_ps <= {DATA_WIDTH{1'b0}};
        data_ps_vld <= 1'b0;
    end
    else begin
        data_ps <= data_ns_vld ? data_ns : data_ps;
        data_ps_vld <= data_ns_vld;
    end
end

endmodule