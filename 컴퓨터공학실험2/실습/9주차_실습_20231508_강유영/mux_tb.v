`timescale 1ns / 1ps


module mux_tb;

reg a,b,A,B,C,D;

wire Out;

mux u_test(
.a(a ),
.b(b ),
.A(A ),
.B(B ),
.C(C ), 
.D(D ),
.Out(Out )  
);

initial begin
    a = 1'b0;
    b = 1'b0;
    A = 1'b0;
    B = 1'b0;
    C = 1'b0;
    D = 1'b0; 
        
end

always@(a or b or A or B or C or D) begin
    a <= #60 ~a;
    b <= #100 ~b;
    A <= #150 ~A; 
    B <= #200 ~B; 
    C <= #250 ~C; 
    D <= #300 ~D; 
end

initial begin
    #1000
    $finish;
end



endmodule