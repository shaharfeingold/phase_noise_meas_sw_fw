


module fifo(clk, rstn, in_data, in_data_vld, out_data_rdy, out_data_vld, 
            out_data, out_data_ptr, fifo_size, flush_fifo, event_overflow, event_underrun);
//parameters
parameter DATA_WIDTH = 32; 
parameter FIFO_SIZE = 1024;
parameter integer FIFO_SIZE_WIDTH = $clog2(FIFO_SIZE);

//ports
input clk, rstn;
input [DATA_WIDTH-1 : 0] in_data;
input in_data_vld;

output [DATA_WIDTH-1 : 0] out_data;
input out_data_vld;
output [FIFO_SIZE_WIDTH-1 : 0] out_data_ptr;
output out_data_rdy;

output reg [FIFO_SIZE_WIDTH : 0] fifo_size;
output event_overflow;
output event_underrun; //event set to 1 when trying to pop from empty fifo

input flush_fifo;

//implementaion
reg [DATA_WIDTH-1 : 0] data [0 : FIFO_SIZE]; //stores the data in fifo //todo shahar later switch the implement using ff to ease code + ease debug
reg [FIFO_SIZE_WIDTH-1 : 0] current_data_ptr;
wire [FIFO_SIZE_WIDTH-1 : 0] current_data_ptr_ns;
wire [FIFO_SIZE_WIDTH : 0] fifo_size_ns;
reg [FIFO_SIZE_WIDTH-1 : 0] fifo_head_ptr;
wire [FIFO_SIZE_WIDTH-1 : 0] fifo_head_ptr_ns;
reg [FIFO_SIZE_WIDTH-1 : 0] fifo_data_out_ptr;
wire [FIFO_SIZE_WIDTH-1 : 0] fifo_data_out_ptr_ns;
wire internal_reset = rstn & (~flush_fifo);

integer i;
always @(posedge clk) begin
    // if (~rstn) begin
    if (~internal_reset) begin
        for(i=0 ; i < FIFO_SIZE; i = i+1) begin
            data[i] <= {DATA_WIDTH{1'b0}};
        end 
        current_data_ptr <= {FIFO_SIZE_WIDTH{1'b0}};
        fifo_size <= {FIFO_SIZE_WIDTH{1'b0}};
        fifo_head_ptr <= {FIFO_SIZE_WIDTH{1'b0}};
        fifo_data_out_ptr <= {FIFO_SIZE_WIDTH{1'b0}};
    end
    else begin
        //push to fifo
        data[current_data_ptr] <= in_data_vld ? in_data : data[current_data_ptr];
        current_data_ptr <= current_data_ptr_ns == (FIFO_SIZE) ? {FIFO_SIZE_WIDTH{1'b0}} : current_data_ptr_ns;  //for cyclic implementaion
        //pop from fifo
        fifo_head_ptr <= fifo_head_ptr == (FIFO_SIZE) ? {FIFO_SIZE_WIDTH{1'b0}} : fifo_head_ptr_ns;
        //misc
        fifo_size <= fifo_size_ns;
    end
end

//push to fifo
assign current_data_ptr_ns = in_data_vld ? current_data_ptr + {{FIFO_SIZE_WIDTH-1{1'b0}},1'b1} : current_data_ptr;
assign event_overflow = fifo_size > FIFO_SIZE;
//pop from fifo
assign fifo_head_ptr_ns = out_data_vld & out_data_rdy ? fifo_head_ptr +  {{FIFO_SIZE_WIDTH-1{1'b0}},1'b1} : fifo_head_ptr;
//misc
assign fifo_size_ns = in_data_vld & out_data_vld & out_data_rdy ? fifo_size :
                      in_data_vld ? fifo_size + {{FIFO_SIZE_WIDTH-1{1'b0}},1'b1} : 
                      out_data_vld & out_data_rdy ? fifo_size - {{FIFO_SIZE_WIDTH{1'b0}},1'b1} :
                      fifo_size;
assign out_data = data[fifo_head_ptr];
assign out_data_ptr = fifo_head_ptr;
assign out_data_rdy = (fifo_size > 0);
assign event_underrun = out_data_vld & ~(|fifo_size);
//todo shahar review fifo size width  - I add extra bit to maintain the fifo size itself and not - 1
endmodule
