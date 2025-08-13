`timescale 1ns / 1ps

module bcd(
    input C_in,
    input [3:0] a,
    input [3:0] b,
    output [3:0] sum,
    output C_out
    );

    wire [3:0] s;
    wire [3:0] c;

    assign s[0] = (a[0] ^ b[0]) ^ C_in;
    assign c[0] = (C_in && (a[0] ^ b[0])) || (a[0] && b[0]);

    assign s[1] = (a[1] ^ b[1]) ^ c[0];
    assign c[1] = (c[0] && (a[1] ^ b[1])) || (a[1] && b[1]);

    assign s[2] = (a[2] ^ b[2]) ^ c[1];
    assign c[2] = (c[1] && (a[2] ^ b[2])) || (a[2] && b[2]);

    assign s[3] = (a[3] ^ b[3]) ^ c[2];
    assign c[3] = (c[2] && (a[3] ^ b[3])) || (a[3] && b[3]);

    assign C_out = (c[3] || (s[2] && s[3]) || (s[1] && s[3]));

    wire C1, C2, C3;
    assign sum[0] = s[0];
    assign C1 = (0 && (s[0] ^ 0)) || (s[0] && 0);

    assign sum[1] = (s[1] ^ C_out) ^ C1;
    assign C2 = (C1 && (s[1] ^ C_out)) || (s[1] && C_out);

    assign sum[2] = (s[2] ^ C_out) ^ C2;
    assign C3 = (C2 && (s[2] ^ C_out)) || (s[2] && C_out);

    assign sum[3] = (s[3] ^ 0) ^ C3;

endmodule
