// Emre Duman
// Pipeline Top-level design
//
`include "./common/config.sv"
`include "./common/constants.sv"

					
module top (
   input logic 		 clock,
   input logic 		 reset,
	output [9:0]funct12_debug,
	output memory_stall,
	output [6:0]opcode_debug,
	output damem_rw
);

logic [6:0] Opcode;
logic [9:0] funct12;
logic MemRead,ALUSrc,Jump,SAJ,Branch_equal,Branch_not_equal,Branch_greater,Branch_less,Jump_Register,stall,insert_nope;
logic [1:0] MemWrite;
logic [1:0] MemToReg;
logic [2:0] RegWrite;
logic [3:0] ALUOp1;
logic [3:0] ALUOp2;
logic [1:0] exmem_memwrite;
assign opcode_debug = Opcode;

   control u0(
	.Opcode                 	(Opcode),
	.funct12                	(funct12),
	.MemRead					(MemRead),
	.MemWrite			 	 	(MemWrite),
	.ALUSrc	       				(ALUSrc),
	.Branch_equal				(Branch_equal),
	.Branch_not_equal			(Branch_not_equal),
	.Branch_greater				(Branch_greater),
	.Branch_less				(Branch_less),
	.Jump						(Jump),
	.Jump_Register				(Jump_Register),
	.SAJ						(SAJ),
	.MemToReg					(MemToReg),
	.RegWrite					(RegWrite),
	.ALUOp1						(ALUOp1),
	.ALUOp2						(ALUOp2),
	.stall						(stall),
	.funct12_out_debug			(funct12_debug)
    );
   
   datapath u1(
	
	.Opcode                 	(Opcode),
	.funct12_out            	(funct12),
	.clock                  	(clock),
	.reset                		(reset),
	.MemRead					(MemRead),
	.MemWrite			 	 	(MemWrite),
	.ALUSrc	       				(ALUSrc),
	.Branch_equal				(Branch_equal),
	.Branch_not_equal			(Branch_not_equal),
	.Branch_greater				(Branch_greater),
	.Branch_less				(Branch_less),
	.Jump						(Jump),
	.Jump_Register				(Jump_Register),
	.SAJ						(SAJ),
	.MemToReg					(MemToReg),
	.RegWrite					(RegWrite),
	.ALUOp1						(ALUOp1),
	.ALUOp2						(ALUOp2),
	.stall						(stall),
	.insert_nope				(insert_nope),
	
	.icache_address			(icache_address),
	.icache_data_out		(icache_data_out),
	.icache_data_ready		(icache_data_ready),
	.icache_valid			(icache_valid),
	
	.dcache_address			(dcache_address), 
	.dcache_data_in			(dcache_data_in), 
	.dcache_byte_en			(dcache_byte_en), 
	.dcache_rw				(dcache_rw), 
	.dcache_valid			(dcache_valid), 
	.dcache_data_out		(dcache_data_out), 
	.dcache_data_ready		(dcache_data_ready),
	.dram_busy				(dram_busy),
	.memory_stall			(memory_stall)
    );
	
	logic [`DRAM_ADDRESS_SIZE-1:0]  icache_address; // from datapath446 (PC) to icache_controller
	logic icache_valid;
	logic [`DRAM_WORD_SIZE-1:0]icache_data_out; 	// data to CPU (cache->CPU)
	logic  icache_data_ready; 			// data to CPU ready (cache->CPU)
	logic [`DRAM_ADDRESS_SIZE-1:0] imem_address;	// icache request address (cache->memory)	
    logic imem_valid; 					// request to memory valid (cache->memory)
	logic dram_busy;
	logic dram_port1_we;
	logic [`DRAM_WORD_SIZE-1:0] dram_port1_read_data[`DRAM_BLOCK_SIZE-1:0];
	logic [`DRAM_WORD_SIZE-1:0] dram_port1_write_data[`DRAM_BLOCK_SIZE-1:0];
	logic dram_port1_acknowledge; 
	logic [`DRAM_ADDRESS_SIZE-1:0] dcache_address; 	// cpu request address (CPU->cache)
	logic [`DRAM_WORD_SIZE-1:0] dcache_data_in;    	// cpu request data (CPU->cache)
	logic [`DRAM_WORD_SIZE/8-1:0]   dcache_byte_en; // cpu request byte enable (CPU->cache)
	logic  dcache_rw;   	// cpu R/W request (CPU->cache)
	logic  dcache_valid; 	// cpu request valid (CPU->cache)
	logic [`DRAM_WORD_SIZE-1:0] dcache_data_out; 	// data to CPU (cache->CPU)
	logic dcache_data_ready; // data to CPU ready (cache->CPU)
	logic [`DRAM_ADDRESS_SIZE-1:0] damem_address; 	// cache request address (cache->memory)
	logic [`DRAM_WORD_SIZE-1:0]  damem_data_out[`DRAM_BLOCK_SIZE-1:0]; // memory write data (cache->memory)
	//logic  damem_rw;		// R/W request to memory (cache->memory)
	logic  damem_valid;		// request to memory valid (cache->memory)
	logic [`DRAM_WORD_SIZE-1:0]   dram_port2_read_data[`DRAM_BLOCK_SIZE-1:0];
	logic dram_port2_acknowledge;
	
	
	assign memory_stall = dram_busy || (icache_valid && !icache_data_ready) || (dcache_valid && !dcache_data_ready);
		
	
	icache_controller u2(.clock(clock), .reset(reset), .icache_address(icache_address), .icache_valid(icache_valid), 
				.icache_data_out(icache_data_out), .icache_data_ready(icache_data_ready), .mem_data_in (dram_port1_read_data),
				.mem_ready(dram_port1_acknowledge), .mem_address(imem_address), .mem_valid(imem_valid));
				
	dcache_controller u3(.clock(clock), .reset(reset), .dcache_address(dcache_address), .dcache_data_in(dcache_data_in), 
				.dcache_byte_en (dcache_byte_en), .dcache_rw(dcache_rw), .dcache_valid(dcache_valid), .dcache_data_out(dcache_data_out),
				.dcache_data_ready(dcache_data_ready), .mem_address(damem_address), .mem_data_out(damem_data_out), .mem_rw(damem_rw),  
				.mem_valid(damem_valid), .mem_data_in(dram_port2_read_data), .mem_ready(dram_port2_acknowledge)  );
				
	/* verilator lint_off PINMISSING */			
	dram_controller u4(.clock(clock), .reset(reset), .dram_busy(dram_busy), .dram_port1_read_data(dram_port1_read_data), 
				.dram_port1_acknowledge(dram_port1_acknowledge), .dram_port1_address(imem_address), .dram_port1_request(imem_valid),
				.dram_port2_address(damem_address), .dram_port2_write_data(damem_data_out), .dram_port2_we(damem_rw),
				.dram_port2_request(damem_valid), .dram_port2_read_data(dram_port2_read_data), .dram_port2_acknowledge(dram_port2_acknowledge)
				);
	/* verilator lint_on PINMISSING */			
	
	/*
	icache_controller icache_controller(
	.clock                  (clock),
	.reset                	(reset),
	.icache_address			(icache_address),
	.icache_data_out		(icache_data_out),
	.icache_data_ready		(icache_data_ready),
	.icache_valid			(icache_valid)
	);
   */
endmodule