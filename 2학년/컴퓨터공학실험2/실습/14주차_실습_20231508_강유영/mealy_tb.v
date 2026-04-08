`timescale 1ns / 1ps

module mealy_tb;

reg clk,reset,x;
wire[3:0] seq;
wire z;

mealy u_test(
    .clk(clk ),
    .reset(reset ),
    .x(x ),
    .z(z ),
    .seq(seq )
);

initial clk = 1'b0;
initial reset = 1'b1;
initial x = 1'b0;


always clk = #20 ~clk;

always@(reset) begin
    reset = #30 ~reset;
end

always@(x) begin
    x = #90 ~x;
    x = #20 ~x;
    
    x = #20 ~x;
    x = #20 ~x;
    
    x = #60 ~x;
    x = #20 ~x;
    
    x = #20 ~x;
    x = #20 ~x;
    
    x = #100 ~x;
    x = #20 ~x;
    
    x = #20 ~x;
    x = #20 ~x;
    
    x = #60 ~x;
    x = #20 ~x;
end

initial begin
    #760
    $finish;
end

endmodule