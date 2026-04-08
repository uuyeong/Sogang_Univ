`timescale 1ns / 1ps


module Half_Subtractor_tb;

reg A,B;

wire b,D;

Half_Subtractor test(
.A(A),
.B(B),

.b(b),
.D(D)
);

initial begin
    A = 1'b0;
    B = 1'b0;
end

always@(A or B) begin
    A <= #60 ~A;
    B <= #100 ~B;
end

initial begin
    #1000
    $finish;
end



endmodule
