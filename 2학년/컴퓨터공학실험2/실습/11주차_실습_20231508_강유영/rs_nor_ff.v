`timescale 1ns / 1ps

module rs_nor_ff(
    input r,s,clock,
    output q,nq
    );
    
    assign q = ~(r&clock | nq);
    assign nq = ~(s&clock | q);

    
endmodule