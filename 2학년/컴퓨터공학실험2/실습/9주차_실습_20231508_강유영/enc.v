`timescale 1ns / 1ps


module enc(

input A,B,C,D,
output E0,E1
    );
    
assign E0 = C | D;
assign E1 = B | D;


endmodule