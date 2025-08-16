`timescale 1ns / 1ps

module bit(
    input a,
    input b,
    output equal,
    output equal_not,
    output left_big,
    output right_big
    );
    
assign equal = ~(a^b);
assign equal_not = a^b;
assign left_big = a&&(~b);
assign right_big = (~a)&&b;
    
endmodule