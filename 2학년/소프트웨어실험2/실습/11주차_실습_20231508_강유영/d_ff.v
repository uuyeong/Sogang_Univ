`timescale 1ns / 1ps

module d_ff(
    input d, clock,
    output q, nq
);

    wire s, r;
    wire q_internal, nq_internal;

    reg d_clock;
    always @(posedge clock) begin
        d_clock <= d;
    end

    assign s = d_clock;
    assign r = ~d_clock;

    assign q_internal = ~(nq_internal | r);  
    assign nq_internal = ~(q_internal | s);

    assign q = q_internal;
    assign nq = nq_internal;

endmodule