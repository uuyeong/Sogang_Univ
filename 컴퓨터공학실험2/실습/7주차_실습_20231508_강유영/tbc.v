`timescale 1ns / 1ps


module tbc(

input a,b,c,d,
output f1,f2,f3
    );
    
assign f1 = (a&~c) | (b&~c&~d) | (a&b&~d);
assign f2 = (~a&~b&~c&~d) | (~a&b&~c&d) | (a&~b&c&~d) | (a&b&c&d);
assign f3 = (~a&c) | (~a&~b&d) | (~b&c&d);




endmodule