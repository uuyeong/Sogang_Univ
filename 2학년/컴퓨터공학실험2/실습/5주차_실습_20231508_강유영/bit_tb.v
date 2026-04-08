`timescale 1ns / 1ps

module bit_tb;
reg a,b;
wire equal,equal_not,left_big,right_big;

bit test (
    .a(a ),
    .b(b ),
    .equal(equal ),
    .equal_not(equal_not ),
    .left_big(left_big ),
    .right_big(right_big )
);

initial begin
    a = 1'b0;
    b = 1'b0;
end

always@(a or b) begin
    a <= #60 ~a;
    b <= #100 ~b;
end

initial begin
    #1000
    $finish;
end

endmodule