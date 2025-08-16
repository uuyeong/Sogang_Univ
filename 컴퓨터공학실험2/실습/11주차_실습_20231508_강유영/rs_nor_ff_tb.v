`timescale 1ns / 1ps

module rs_nor_ff_tb;
reg r,s,clock;
wire q,nq;

rs_nor_ff u_inv(
    .r(r ),
    .s(s ),
    .clock(clock ),
    .q(q ),
    .nq(nq )
);

initial begin
    r = 1'b0;
    s = 1'b0;
    clock = 1'b0;
end

always begin
    s = #20 ~s;
    r = #22 ~r;
    clock = #24 ~clock;
end

initial begin
    #1000
    $finish;
end

endmodule