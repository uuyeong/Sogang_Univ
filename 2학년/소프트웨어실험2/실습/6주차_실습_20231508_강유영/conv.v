`timescale 1ns / 1ps

module conv(
    input a1,
    input a2,
    input a3,
    input a4,
    
    output b1,
    output b2,
    output b3,
    output b4
    );
    
    assign b1 = (a2&a3) | (a2&a4) | a1;
    assign b2 = a1 | (a2&~a4) | (a2&a3);
    assign b3 = a1 | (~a2&a3) | (a2&~a3&a4);
    assign b4 = a4;
    
endmodule