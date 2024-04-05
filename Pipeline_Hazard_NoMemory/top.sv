// Emre Duman
// Pipeline Top-level design
//

					
module top (
   input logic 		 clock,
	input logic 		 reset,
	output [9:0]funct12_debug,
	output insert_nope
);

logic [6:0] Opcode;
logic [9:0] funct12;
logic MemRead,ALUSrc,Jump,SAJ,Branch_equal,Branch_not_equal,Branch_greater,Branch_less,Jump_Register,stall;
logic [1:0] MemWrite;
logic [1:0] MemToReg;
logic [2:0] RegWrite;
logic [3:0] ALUOp1;
logic [3:0] ALUOp2;




   control u0(
	.Opcode                 (Opcode),
	.funct12                (funct12),
	.MemRead					   (MemRead),
	.MemWrite			 	 	(MemWrite),
	.ALUSrc	       			(ALUSrc),
	.Branch_equal				(Branch_equal),
	.Branch_not_equal			(Branch_not_equal),
	.Branch_greater			(Branch_greater),
	.Branch_less				(Branch_less),
	.Jump							(Jump),
	.Jump_Register				(Jump_Register),
	.SAJ							(SAJ),
	.MemToReg					(MemToReg),
	.RegWrite					(RegWrite),
	.ALUOp1						(ALUOp1),
	.ALUOp2						(ALUOp2),
	.stall						(stall),
	.funct12_out_debug			(funct12_debug)
    );
   
   datapath u1(
	
	.Opcode                 (Opcode),
	.funct12_out            (funct12),
	.clock                  (clock),
    .reset                	(reset),
	.MemRead					   (MemRead),
	.MemWrite			 	 	(MemWrite),
	.ALUSrc	       			(ALUSrc),
	.Branch_equal				(Branch_equal),
	.Branch_not_equal			(Branch_not_equal),
	.Branch_greater			(Branch_greater),
	.Branch_less				(Branch_less),
	.Jump							(Jump),
	.Jump_Register				(Jump_Register),
	.SAJ							(SAJ),
	.MemToReg					(MemToReg),
	.RegWrite					(RegWrite),
	.ALUOp1						(ALUOp1),
	.ALUOp2						(ALUOp2),
	.stall						(stall),
	.prediction_bit				(1'b0),
	.BHT_pc						(0),
	.insert_nope				(insert_nope)
       
    );
   
endmodule