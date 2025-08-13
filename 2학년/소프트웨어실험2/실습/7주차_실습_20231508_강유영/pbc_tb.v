`timescale 1ns / 1ps


module pbc_tb;

reg a,b,c,d,p;

wire pec;

pbc u_test(
.a(a ),
.b(b ),
.c(c ),
.d(d ),
.p(p ),

.pec(pec )
);

initial begin
    a = 1'b0;
    b = 1'b0;
    c = 1'b0;
    d = 1'b0;
    p = 1'b0;
end

always@(a or b or c or d or p) begin
    a <= #60 ~a;
    b <= #100 ~b;
    c <= #150 ~c;
    d <= #200 ~d;
    p <= #250 ~p;
end

initial begin
    #1000
    $finish;
end



endmodule
