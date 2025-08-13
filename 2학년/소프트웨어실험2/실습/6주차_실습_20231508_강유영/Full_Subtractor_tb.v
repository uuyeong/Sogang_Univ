`timescale 1ns / 1ps


module Full_Subtractor_tb;

reg A,B,bn_1;

wire D,bn;

Full_Subtractor test(
.A(A),
.B(B),
.bn_1(bn_1),

.D(D),
.bn(bn)
);

initial begin
    A = 1'b0;
    B = 1'b0;
    bn_1 = 1'b0;
end

always@(A or B or bn_1) begin
    A <= #60 ~A;
    B <= #100 ~B;
    bn_1 <= #140 ~bn_1;
end

initial begin
    #1000
    $finish;
end



endmodule
