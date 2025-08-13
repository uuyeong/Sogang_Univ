`timescale 1ns / 1ps


module seg_tb;

reg a,b,c,d;

wire A,B,C,D,E,F,G,Dp,digit;

seg u_test(
.a(a ),
.b(b ),
.c(c ),
.d(d ),

.A(A ),
.B(B ),
.C(C ),
.D(D ),
.E(E ),
.F(F ),
.G(G ),
.Dp(Dp ),
.digit(digit )
);

initial begin
    a = 1'b0;
    b = 1'b0;
    c = 1'b0;
    d = 1'b0;
end

always@(a or b or c or d) begin
    a <= #60 ~a;
    b <= #100 ~b;
    c <= #150 ~c;
    d <= #200 ~d;
end

initial begin
    #1000
    $finish;
end



endmodule