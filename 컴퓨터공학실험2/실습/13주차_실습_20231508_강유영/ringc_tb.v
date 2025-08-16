`timescale 1ns / 1ps

module ringc_tb;

reg clk,reset;
wire[3:0] out;

ringc u_test(
    .clk(clk ),
    .reset(reset ),
    .out(out )
);

initial clk = 1'b0;
initial reset = 1'b0;


always clk = #20 ~clk;

always@(reset) begin
    reset = #330 ~reset;
    reset = #20 ~reset;
end

initial begin
    #380
    $finish;
end

endmodule