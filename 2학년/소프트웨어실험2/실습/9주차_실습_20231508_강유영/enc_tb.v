`timescale 1ns / 1ps


module enc_tb;

reg A,B,C,D;

wire E0,E1;

enc u_test(
.A(A ),
.B(B ),
.C(C ),
.D(D ),
 
.E0(E0 ),
.E1(E1 )
);

initial begin
    A = 1'b0;
    B = 1'b0;
    C = 1'b0;
    D = 1'b0;
 
end

always@(A or B or C or D) begin
    A <= #60 ~A;
    B <= #100 ~B;
    C <= #150 ~C;
    D <= #200 ~D; 
end

initial begin
    #1000
    $finish;
end



endmodule