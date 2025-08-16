`timescale 1ns / 1ps

module four_count_tb;

reg clock,reset,x;
wire [3:0]out,z;

four_count u_test(
    .clock(clock ),
    .reset(reset ),
    .x(x ),
    .out(out ),
    .z(z )
);

initial clock = 1'b0;
initial reset = 1'b1;
initial x = 1'b0;

always clock = #20 ~clock;

always@(reset) begin
    reset = #50 ~reset;
end

always@(x) begin
    x = #110 ~x;
    x = #20 ~x;
    x = #60 ~x;
    x = #20 ~x;
    x = #60 ~x;
    x = #20 ~x;
    x = #60 ~x;
    x = #20 ~x;
    x = #60 ~x;
    x = #20 ~x;
    x = #60 ~x;
    x = #20 ~x;
    x = #60 ~x;
    x = #20 ~x;
    x = #60 ~x;
    x = #20 ~x;
    x = #60 ~x;
    x = #20 ~x;
    x = #60 ~x;
    x = #20 ~x;
end

initial begin
    #860
    $finish;
end

endmodule