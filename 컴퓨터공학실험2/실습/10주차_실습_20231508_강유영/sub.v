`timescale 1ns / 1ps

module sub(
    input b_in,
    input [3:0] A,
    input [3:0] B,
    output [3:0] D,
    output b4
    );

    wire [3:0] borrow;

    assign D[0] = (A[0] ^ B[0]) ^ b_in;
    assign borrow[1] = ((~(A[0] ^ B[0])) & b_in) | ((~A[0]) & B[0]);

    assign D[1] = (A[1] ^ B[1]) ^ borrow[1];
    assign borrow[2] = ((~(A[1] ^ B[1])) & borrow[1]) | ((~A[1]) & B[1]);

    assign D[2] = (A[2] ^ B[2]) ^ borrow[2];
    assign borrow[3] = ((~(A[2] ^ B[2])) & borrow[2]) | ((~A[2]) & B[2]);

    assign D[3] = (A[3] ^ B[3]) ^ borrow[3];
    assign b4 = ((~(A[3] ^ B[3])) & borrow[3]) | ((~A[3]) & B[3]);

endmodule
