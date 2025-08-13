`timescale 1ns / 1ps


module input4_or(
    input aa,
    input bb,
    input cc,
    input dd,
    
    output ee,
    output ff,
    output gg
    );
    
    assign ee = aa|bb;
    assign ff = ee|cc;
    assign gg = ff|dd;
    
endmodule
