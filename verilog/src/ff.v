
module ff(clk, rstn, data_ns, data_vld, data_ps);
//parameters
parameter DATA_WIDTH = 32;

//ports
input clk, rstn

input [DATA_WIDTH-1 : 0] data_ns;
input data_vld;

output reg [DATA_WIDTH-1 : 0] data_ps;

//implemention
always @(posedge clk) begin
    if (~rstn) begin
        data_ps <= {DATA_WIDTH{1'b0}};
    end
    else begin
        data_ps <= data_ns ? data_ns : data_ps;
    end
end

endmodule