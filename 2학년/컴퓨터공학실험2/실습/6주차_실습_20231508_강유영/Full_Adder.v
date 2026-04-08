`timescale 1ns / 1ps


module Full_Adder(

input a,b,c_in,
output s,c_out
    );
    
    assign s = ~((~a&b)|(a&~b))&c_in|((~a&b)|(a&~b))&~c_in;
    assign c_out = ((~a&b)|(a&~b))&c_in | a&b;
    
endmodule