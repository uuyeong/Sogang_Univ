`timescale 1ns / 1ps


module tbc_tb;

reg a,b,c,d;

wire f1,f2,f3;

tbc u_test(
.a(a ),
.b(b ),
.c(c ),
.d(d ),

.f1(f1 ),
.f2(f2 ),
.f3(f3 )
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