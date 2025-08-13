`timescale 1ns / 1ps


module Half_Adder(

input a,b,
output s,c
    );
    
    assign s = (~a)&b|a&(~b);
    assign c = a&b;
    
endmodule
