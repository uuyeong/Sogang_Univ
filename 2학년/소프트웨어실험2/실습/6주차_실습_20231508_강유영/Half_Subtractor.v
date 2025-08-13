`timescale 1ns / 1ps


module Half_Subtractor(

input A,B,
output b,D
    );
    
    assign D = (~A&B) | (A&~B);
    assign b = ~A&B;
    
endmodule