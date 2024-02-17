module async_fifo(rst, rd_clk, read_req, data_out_vld, data_out, fifo_empty, wr_clk, data_in_vld, data_in, fifo_full);
//parameters
parameter DATA_WIDTH = 32;
parameter FIFO_DEPTH = 8;
parameter FIFO_DEPTH_WIDTH = $clog2(FIFO_DEPTH);

//ports
input rst; //generate a rest for each domain with it clock speratly
// read i/f
input rd_clk;
input read_req;
output data_out_vld;
output [DATA_WIDTH-1 : 0] data_out;
output fifo_empty;

//write i/f
input wr_clk;
input data_in_vld;
input [DATA_WIDTH-1 : 0] data_in;
output fifo_full;

//implementation

reg [DATA_WIDTH-1 : 0] memory [0 : FIFO_DEPTH-1];

//write_domain - write clock domain
reg [FIFO_DEPTH_WIDTH: 0] write_ptr;
wire [FIFO_DEPTH_WIDTH: 0] write_ptr_next;
reg fifo_full_p;
wire full = (write_ptr_next_gray ==  {~read_ptr_gray_wr_sync[FIFO_DEPTH_WIDTH:FIFO_DEPTH_WIDTH-1],read_ptr_gray_wr_sync[FIFO_DEPTH_WIDTH-2:0]}); //this line asuuming depth should be at least 3 bits width
//wire fifo_full_aux_wraparound = write_ptr_gray[FIFO_DEPTH_WIDTH-1:FIFO_DEPTH_WIDTH-1] ^ read_ptr_gray_wr_sync[FIFO_DEPTH_WIDTH-1 : FIFO_DEPTH_WIDTH-1];

assign write_ptr_next =  write_ptr + {{FIFO_DEPTH_WIDTH{1'b0}},1'b1} ;
generate
genvar i;
always @(posedge wr_clk ) begin
    if (rst) begin
        write_ptr <= {FIFO_DEPTH_WIDTH{1'b0}};
        for (integer i=0 ; i < FIFO_DEPTH ; i = i +1) begin
            memory[i] <= {DATA_WIDTH{1'b0}};
            fifo_full_p <= 1'b1;
        end
    end
    else begin
        //write_ptr <= (~fifo_full_p & data_in_vld) ? write_ptr_next : write_ptr;
        write_ptr <= (~full & data_in_vld) ? write_ptr_next : write_ptr;
        memory[write_ptr[FIFO_DEPTH_WIDTH-1:0]] <= (~fifo_full_p & data_in_vld) ? data_in : memory[write_ptr[FIFO_DEPTH_WIDTH-2:0]];
        // fifo_full_p <= fifo_full_aux_wraparound &  (write_ptr_next_gray[FIFO_DEPTH_WIDTH-2:0] == read_ptr_gray_wr_sync[FIFO_DEPTH_WIDTH-2:0]);
        fifo_full_p <= full;
    end
    
end

endgenerate

//read_domain - read clock domain
reg [FIFO_DEPTH_WIDTH: 0] read_ptr;
wire [FIFO_DEPTH_WIDTH: 0] read_ptr_next;
reg [DATA_WIDTH-1 : 0] data_out_p;
reg data_out_vld_p;
reg empty_p;
wire empty_pp = (read_ptr_gray == write_ptr_gray_rd_sync);

assign data_out = data_out_p;
assign data_out_vld = data_out_vld_p;

assign read_ptr_next =  read_ptr + {{FIFO_DEPTH_WIDTH{1'b0}},1'b1} ;

always @(posedge rd_clk ) begin
    if (rst) begin
        read_ptr <= {FIFO_DEPTH_WIDTH{1'b0}};
        data_out_p <= {DATA_WIDTH{1'b0}};
        data_out_vld_p <= 1'b0;
        empty_p <= 1'b1;
    end
    else begin
        read_ptr <= (~empty_p & read_req) ? read_ptr_next : read_ptr;
        data_out_p <= (~empty_p & read_req) ? memory[read_ptr[FIFO_DEPTH_WIDTH-2:0]] : {DATA_WIDTH{1'b0}};
        data_out_vld_p <= (~empty_p & read_req) ? 1'b1 : 1'b0;
        empty_p <= empty_pp;
    end
    
end

assign fifo_empty = empty_pp;
//assign fifo_full = fifo_full_p;
assign fifo_full = full;

wire [FIFO_DEPTH_WIDTH: 0] read_ptr_next_gray;
wire [FIFO_DEPTH_WIDTH: 0] read_ptr_gray;

graycode2bin #(.DATA_WIDTH(FIFO_DEPTH_WIDTH)) graycode2bin_rd_ptr(
    .in_data(read_ptr),
    .out_data(read_ptr_gray)
);

graycode2bin #(.DATA_WIDTH(FIFO_DEPTH_WIDTH)) graycode2bin_rd_ptr_next(
    .in_data(read_ptr_next),
    .out_data(read_ptr_next_gray)
);

wire [FIFO_DEPTH_WIDTH : 0] write_ptr_gray;
wire [FIFO_DEPTH_WIDTH : 0] write_ptr_next_gray;

graycode2bin #(.DATA_WIDTH(FIFO_DEPTH_WIDTH)) graycode2bin_wr_ptr_next(
    .in_data(write_ptr_next),
    .out_data(write_ptr_next_gray)
);

graycode2bin #(.DATA_WIDTH(FIFO_DEPTH_WIDTH)) graycode2bin_wr_ptr(
    .in_data(write_ptr),
    .out_data(write_ptr_gray)
);


//sync from write clock to read clock
wire [FIFO_DEPTH_WIDTH: 0] write_ptr_gray_rd_sync;
sync #(.SYNC_WIDTH(2), .FIFO_DEPTH(FIFO_DEPTH)) sync_wr2rd(
    .rst(rst),
    .clk(rd_clk),
    .data_in(write_ptr_gray),
    .data_out(write_ptr_gray_rd_sync)
);

//sync from read clock to write clock
wire [FIFO_DEPTH_WIDTH : 0] read_ptr_gray_wr_sync;
sync #(.SYNC_WIDTH(2), .FIFO_DEPTH(FIFO_DEPTH)) sync_rd2wr(
    .rst(rst),
    .clk(wr_clk),
    .data_in(read_ptr_gray),
    .data_out(read_ptr_gray_wr_sync)
);
 

endmodule


module sync(rst, clk, data_in, data_out);
//parameters
parameter SYNC_WIDTH = 2;
parameter FIFO_DEPTH = 3;
parameter FIFO_DEPTH_WIDTH = $clog2(FIFO_DEPTH);

//ports
input [FIFO_DEPTH_WIDTH: 0] data_in;
output [FIFO_DEPTH_WIDTH: 0] data_out;
input rst; //active high
input clk;

//implementation
reg [FIFO_DEPTH_WIDTH: 0] shiftreg [0:SYNC_WIDTH-1];
generate
genvar i;
always @(posedge clk ) begin
    if (rst) begin
        for (integer i=0 ; i < SYNC_WIDTH ; i = i + 1) begin
            shiftreg[i] <= {FIFO_DEPTH_WIDTH+1{1'b0}};
        end
    end
    else begin
        for (integer i=0 ; i < SYNC_WIDTH-1 ; i = i + 1) begin
            shiftreg[i + 1] <= shiftreg[i];
        end
        shiftreg[0] <= data_in;
    end
end

assign data_out = shiftreg[SYNC_WIDTH-1];

endgenerate

endmodule

module graycode2bin(in_data, out_data);
//parameters
parameter DATA_WIDTH = 32;

//ports
input [DATA_WIDTH: 0] in_data;
output [DATA_WIDTH: 0] out_data;

//implementation
assign out_data = (in_data >> 1) ^ in_data ;
endmodule