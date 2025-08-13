`timescale 1ns / 1ps

module conv_tb;
reg a1,a2,a3,a4;
wire b1,b2,b3,b4;

conv test (
    .a1(a1 ),
    .a2(a2 ),
    .a3(a3 ),
    .a4(a4 ),
    
    .b1(b1 ),
    .b2(b2 ),
    .b3(b3 ),
    .b4(b4 )
);

initial begin
    a1 = 1'b0;
    a2 = 1'b0;
    a3 = 1'b0;
    a4 = 1'b0;
end

always@(a1 or a2 or a3 or a4) begin
    a1 <= #20 ~a1;
    a2 <= #40 ~a2;
    a3 <= #80 ~a3;
    a4 <= #160 ~a4;
end

initial begin
    #320
    $finish;
end

endmodule