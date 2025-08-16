`timescale 1ns / 1ps

module d_ff_tb;
    reg d, clock;
    wire q, nq;

    d_ff u_test (
        .d(d),
        .clock(clock),
        .q(q),
        .nq(nq)
    );

    initial begin
        clock = 1'b0;
        forever #20 clock = ~clock; 
    end

    initial begin
        d = 1'b0;
        #30 d = 1'b1; 
        #40 d = 1'b0;
        #40 d = 1'b1;
        #50 $finish;
    end
endmodule
