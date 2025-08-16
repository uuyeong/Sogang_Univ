`timescale 1ns / 1ps


module Half_Adder_tb;

reg a,b;

wire s,c;

Half_Adder test(
.a(a),
.b(b),

.s(s),
.c(c)
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