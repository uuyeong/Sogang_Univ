`timescale 1ns / 1ps


module seg(

input a,b,c,d,
output A,B,C,D,E,F,G,Dp,digit
    );
    
assign A = ((~b)&(~d)) | ((~a)&c) | (b&c) | (a&(~d)) | ((~a)&b&d) | (a&(~b)&(~c));
assign B = ((~a)&(~b)) | ((~b)&(~d)) | ((~a)&(~c)&(~d)) | ((~a)&c&d) | (a&(~c)&d);
assign C = ((~a)&(~c)) | ((~a)&d) | ((~c)&d) | ((~a)&b) | (a&(~b));
assign D = ((~a)&(~b)&(~d)) | ((~b)&c&d) | (b&(~c)&d) | (b&c&(~d)) | (a&(~c)&(~d));
assign E = ((~b)&(~d)) | (c&(~d)) | (a&c) | (a&b);
assign F = ((~c)&(~d)) | (b&(~d)) | (a&(~b)) | (a&c) | ((~a)&b&(~c));
assign G = ((~b)&c) | (c&(~d)) | (a&(~b)) | (a&d) | ((~a)&b&(~c));


assign Dp = 1;
assign digit = A | B | C | D | E | F | G;


endmodule