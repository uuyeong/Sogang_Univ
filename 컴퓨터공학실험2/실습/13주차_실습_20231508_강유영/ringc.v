`timescale 1ns / 1ps

module ringc(
    input reset,
    input clk,
    output[3:0] out
);

reg[3:0] out = 4'b1000;

always @(posedge clk or posedge reset) begin
    if (reset) begin
        out <= 4'b1000;  
    end
    else begin
        out[3] <= out[0];
        out[2] <= out[3];
        out[1] <= out[2];
        out[0] <= out[1];
    end
end

endmodule