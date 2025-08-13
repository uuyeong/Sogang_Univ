`timescale 1ns / 1ps

module input4_or_tb;

reg aa,bb,cc,dd;

wire ee,ff,gg;
input4_or u_test (
    .aa(aa ),
    .bb(bb ),
    .cc(cc ),
    .dd(dd ),
    
    .ee(ee ),
    .ff(ff ),
    .gg(gg )
);

initial begin
    aa = 1'b0;
    bb = 1'b0;
    cc = 1'b0;
    dd = 1'b0;
end

always@(aa or bb or cc or dd) begin
    aa <= #60 ~aa;
    bb <= #100 ~bb;
    cc <= #150 ~cc;
    dd <= #200 ~dd;
end

initial begin
    #1000
    $finish;
end

endmodule
