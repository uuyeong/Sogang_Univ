`timescale 1ns / 1ps


module demor1_b_tb;

reg a,b,c;

wire d;

demor1_b test(
.a(a),
.b(b),
.c(c),

.d(d)

);

initial begin
    a = 1'b0;
    b = 1'b0;
    c = 1'b0;
end

always@(a or b or c) begin
    a <= #60 ~a;
    b <= #100 ~b;
    c <= #150 ~c;
end

initial begin
    #1000
    $finish;
end



endmodule