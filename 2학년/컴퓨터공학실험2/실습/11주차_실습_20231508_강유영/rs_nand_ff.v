`timescale 1ns / 1ps

module rs_nand_ff(
    input r,s,clock,
    output q,nq
    );
    
    assign q = ~(~(s&clock) & nq);
    assign nq = ~(~(r&clock) & q);

    
endmodule