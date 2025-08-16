`timescale 1ns / 1ps


module Full_Adder_tb;

reg a,b,c_in;

wire s,c_out;

Full_Adder test(
.a(a),
.b(b),
.c_in(c_in),

.s(s),
.c_out(c_out)
);

initial begin
    a = 1'b0;
    b = 1'b0;
    c_in = 1'b0;
end

always@(a or b or c_in) begin
    a <= #60 ~a;
    b <= #100 ~b;
    c_in <= #540 ~c_in;
end

initial begin
    #1000
    $finish;
end



endmodule