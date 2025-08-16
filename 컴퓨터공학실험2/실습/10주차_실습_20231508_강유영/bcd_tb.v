`timescale 1ns / 1ps

module bcd_tb;
    reg C_in;
    reg [3:0] a, b;
    wire [3:0] sum;
    wire C_out;

    bcd u_test (
        .C_in(C_in),
        .a(a),
        .b(b),
        .sum(sum),
        .C_out(C_out)
    );

    initial begin
        C_in = 1'b0;
        a = 4'b0000;
        b = 4'b0000;
    end

    always @(C_in or a or b) begin
        C_in = #10 ~C_in;
        a[0] <= #20 ~a[0];
        a[1] <= #40 ~a[1];
        a[2] <= #80 ~a[2];
        a[3] <= #160 ~a[3];

        b[0] <= #320 ~b[0];
        b[1] <= #640 ~b[1];
        b[2] <= #1280 ~b[2];
        b[3] <= #2560 ~b[3];
    end

    initial begin
        #5120
        $finish;
    end
endmodule
