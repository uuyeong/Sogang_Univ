module add(
    input C_in,
    input [3:0] A,
    input [3:0] B,
    output [3:0] S,
    output C4
    );

wire [3:0] carry;

assign S[0] = (A[0] ^ B[0]) ^ C_in;
assign carry[1] = (C_in & (A[0] ^ B[0])) | (A[0] & B[0]);

assign S[1] = (A[1] ^ B[1]) ^ carry[1];
assign carry[2] = (carry[1] & (A[1] ^ B[1])) | (A[1] & B[1]);

assign S[2] = (A[2] ^ B[2]) ^ carry[2];
assign carry[3] = (carry[2] & (A[2] ^ B[2])) | (A[2] & B[2]);

assign S[3] = (A[3] ^ B[3]) ^ carry[3];
assign C4 = (carry[3] & (A[3] ^ B[3])) | (A[3] & B[3]);

endmodule
