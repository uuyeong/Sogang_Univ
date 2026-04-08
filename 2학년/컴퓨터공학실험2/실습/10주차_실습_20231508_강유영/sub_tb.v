`timescale 1ns / 1ps

module sub_tb;
    reg b_in;
    reg [3:0] A, B;
    wire [3:0] D;
    wire b4;

    sub u_test (
        .b_in(b_in),
        .A(A),
        .B(B),
        .D(D),
        .b4(b4)
    );

    initial begin
        b_in = 1'b0;
        A = 4'b0000;
        B = 4'b0000;
    end

    always @(b_in or A or B) begin
        b_in = #10 ~b_in;
        A[0] <= #20 ~A[0];
        A[1] <= #40 ~A[1];
        A[2] <= #80 ~A[2];
        A[3] <= #160 ~A[3];

        B[0] <= #320 ~B[0];
        B[1] <= #640 ~B[1];
        B[2] <= #1280 ~B[2];
        B[3] <= #2560 ~B[3];
    end

    initial begin
        #5120
        $finish;
    end
endmodule
