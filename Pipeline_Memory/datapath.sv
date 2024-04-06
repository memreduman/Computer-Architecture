`include "./common/config.sv"
`include "./common/constants.sv"

module datapath(input logic clock, reset,  
                input logic ALUSrc, Branch_equal,Branch_not_equal,Branch_greater,Branch_less, Jump,SAJ,Jump_Register,MemRead,
					 input logic [1:0]MemToReg,MemWrite,
					 input logic [2:0]RegWrite,
					 input logic [3:0]ALUOp1,
					 input logic [3:0]ALUOp2,
					 
					 input logic  memory_stall,
					 input logic [`DRAM_WORD_SIZE-1:0] 	  icache_data_out,   // i_cache->CPU
					 input logic  						  icache_data_ready, // i_cache->CPU
					 
					 input logic [`DRAM_WORD_SIZE-1:0] 	  dcache_data_out,   // d_cache->CPU
					 input logic 			  			  dcache_data_ready, // d_cache->CPU
					 input logic                          dram_busy,		 // dram_controller->CPU
					 
					 output logic stall,
					 output logic insert_nope,
					 output logic [6:0]Opcode,
					 output logic [9:0] funct12_out,
					 
					 output logic [`DRAM_ADDRESS_SIZE-1:0] icache_address,    // CPU-> i_cache
					 output logic icache_valid,								  // CPU-> i_cache
					 
					 output logic [`DRAM_ADDRESS_SIZE-1:0] dcache_address,    // CPU-> d_cache
					 output logic [`DRAM_WORD_SIZE-1:0]    dcache_data_in, 	  // CPU-> d_cache
					 output logic [`DRAM_WORD_SIZE/8-1:0]  dcache_byte_en,    // CPU-> d_cache
					 output logic dcache_rw, 								  // CPU-> d_cache
					 output logic dcache_valid                               // CPU-> d_cache
					 
				
				
					 ); 


	parameter PCSTART = 0; //starting address of instruction memory
	parameter D_OFFSET = 2048; //starting address of data memory -> 512
    logic [31:0] PC = PCSTART;
	logic PCSrc;
	logic [31:0]PC_plus_4;
	
	logic [31:0]TargetAddress;
	logic greaterflag;
	logic zeroflag;
	logic flush;
	
	//logic  memory_stall;
	//assign memory_stall = dram_busy || (icache_valid && !icache_data_ready) || (dcache_valid && !dcache_data_ready);
		
	///// Instruction memory, input address and output data bus signal
	
	logic [`DRAM_WORD_SIZE-1:0]instmem_data;
	
	assign icache_address = PC[`DRAM_ADDRESS_SIZE-1:0] ;
    assign icache_valid = 1;
	assign	instmem_data[31:24] = (icache_data_ready)?icache_data_out[7:0]:8'bx;
	assign	instmem_data[23:16] = (icache_data_ready)?icache_data_out[15:8]:8'bx;
	assign	instmem_data[15:8]  = (icache_data_ready)?icache_data_out[23:16]:8'bx;
	assign	instmem_data[7:0]   = (icache_data_ready)?icache_data_out[31:24]:8'bx;
	
	/*
	always_latch begin
	icache_valid <= 1;
		if(!dram_busy)begin
		/*if(reset)begin
			icache_valid <= 0;
			icache_address <= 0;
			instmem_data <= 0;
		end
		else 
		icache_valid <= 1;
		icache_address <= PC[`DRAM_ADDRESS_SIZE-1:0] ;
		
			if(icache_data_ready)begin
			//instmem_data <= icache_data_out;
			instmem_data[31:24] <= icache_data_out[7:0];
			instmem_data[23:16] <= icache_data_out[15:8];
			instmem_data[15:8]  <= icache_data_out[23:16];
			instmem_data[7:0]   <= icache_data_out[31:24];
			end
		end
		
	end
	*/
	////////////////////////////////////////////////////////////////////////
	
	
	//// Data memory, input address and output data bus signals
	
	logic [31:0] datamem_write_data; 
	assign datamem_write_data = ExMem.R_data3;
	
	logic [31:0] datamem_data;
	
	assign dcache_address = ExMem.AluOut1[`DRAM_ADDRESS_SIZE-1:0] + D_OFFSET; 
	//assign dcache_address = ExMem.AluOut1[`DRAM_ADDRESS_SIZE-1:0];
	
	///////////////// Data Read/Write logics are built here ///////////////
	// Data Memory Read Logic
	assign dcache_valid =  (ExMem.MemRead ||(ExMem.MemWrite>0))? 1'b1:1'b0; //(ExMem.MemRead ||(ExMem.MemWrite>0))? 
	assign dcache_rw	= (!ExMem.MemRead &&(ExMem.MemWrite>0))? 1'b1:1'b0;
	
	assign datamem_data[31:24] = (!dram_busy&& ExMem.MemRead && dcache_data_ready)? dcache_data_out[7:0]:8'bx;
	assign datamem_data[23:16] = (!dram_busy&& ExMem.MemRead && dcache_data_ready)? dcache_data_out[15:8]:8'bx;
	assign datamem_data[15:8]  = (!dram_busy&& ExMem.MemRead && dcache_data_ready)? dcache_data_out[23:16]:8'bx;
	assign datamem_data[7:0]   = (!dram_busy&& ExMem.MemRead && dcache_data_ready)? dcache_data_out[31:24]:8'bx;
	
	/*
	always_latch begin

		if(!dram_busy)begin
			if(ExMem.MemRead) begin
			//dcache_valid <= 1;
			//dcache_rw <= 0;
				
				if(dcache_data_ready)begin
		
					datamem_data[31:24] <= dcache_data_out[7:0];
					datamem_data[23:16] <= dcache_data_out[15:8];
					datamem_data[15:8]  <= dcache_data_out[23:16];
					datamem_data[7:0]   <= dcache_data_out[31:24];
				
				end
			end
				else if(ExMem.MemWrite>0) begin 
				//dcache_rw <= 1;
				//dcache_valid <= 1;
				end
		end
		//else
		//dcache_valid <= 0;
	end
	*/
	// Data Memory Write Logic
	always_comb begin
	dcache_byte_en = 4'bx;
	dcache_data_in = 32'bx;
		if(!dram_busy)begin
			case(ExMem.MemWrite) 
			2'b00:;
			2'b01:begin //sw
				dcache_byte_en = 4'b1111;
				/*
				dcache_data_in[31:24] = {<<{datamem_write_data[7:0]}};
			    dcache_data_in[23:16] = {<<{datamem_write_data[15:8]}};
			    dcache_data_in[15:8]  = {<<{datamem_write_data[23:16]}};
				dcache_data_in[7:0]   = {<<{datamem_write_data[31:24]}};
				*/
				dcache_data_in[31:24] = datamem_write_data[7:0];
			    dcache_data_in[23:16] = datamem_write_data[15:8];
			    dcache_data_in[15:8]  = datamem_write_data[23:16];
				dcache_data_in[7:0]   = datamem_write_data[31:24];
				
			end
			2'b10: begin //sh
				dcache_byte_en = 4'b1100;
				dcache_data_in[23:16]   = datamem_write_data[15:8];
				dcache_data_in[31:24]  	= datamem_write_data[7:0];
			end
			2'b11: begin //sb
				dcache_byte_en = 4'b1000;
				dcache_data_in[31:24]   = datamem_write_data[7:0];
			end
			default ;
			endcase
		end
	end

	
//////////////////////////////////////////////////////////////////////////////////////////
	

	
	// IF/ID Pipeline staging register fields can be represented using structure format of System Verilog
	
	struct packed {
	
		logic [31:0] instruction;
		logic [31:0] PCincremented;
		
		//Hazard Detection
		logic [4:0] rs1;
		logic [4:0] rs2;
		logic [4:0] rs3;
		logic [4:0] RegisterRd;
	} IfId;
	
	
	// ID/EX Pipeline staging register
	struct packed {
		logic [31:0] R_data1;
		logic [31:0] R_data2;
		logic [31:0] R_data3;
		integer sign_ext_32;
		logic [4:0] RegisterRd;
		logic [31:0] PCincremented;
		
		//Control Unit Signals
		logic [3:0]ALUOp1;
		logic [3:0]ALUOp2;
		logic [2:0]RegWrite;
		logic ALUSrc;
		logic [1:0]MemWrite;
		logic MemRead;
		logic [1:0]MemToReg;
		
		//Hazard Detection
		logic [4:0] rs1;
		logic [4:0] rs2;
		logic [4:0] rs3;

		

	}IdEx;

	
	// EX/MEM Pipeline staging register
	struct packed {
	
		logic zeroflag;
		logic [31:0] AluOut1; 
		logic [31:0] R_data2;
		logic [31:0] R_data3;
		logic [4:0] RegisterRd;
		logic [31:0] PCincremented;
		
		//Control Unit Signals		
		logic [3:0]ALUOp2;
		logic [2:0]RegWrite;
		logic [1:0]MemWrite;
		logic MemRead;
		logic [1:0]MemToReg;
		
		//Hazard Detection
		logic [4:0] rs1;
		logic [4:0] rs2;
		logic [4:0] rs3;

		
	} ExMem;
	
	
	// MEM/WB Pipeline staging register
	struct packed {
	logic [31:0] AluOut1;
	logic [31:0] AluOut2;
	logic [31:0] Mem_data;
	logic [4:0] RegisterRd;
	logic [31:0] PCincremented;
	
	//Control Unit Signals	
	logic [1:0]MemToReg;
	logic [2:0]RegWrite;
	logic [1:0]MemWrite;
	//Hazard Detection
		logic [4:0] rs1;
		logic [4:0] rs2;
		logic [4:0] rs3;

		
	} memWB;
	
	
	//// Register File description
	logic [31:0] RF[31:0];
	logic [31:0] da; //read data 1
	logic [31:0] db; //read data 2
	logic [31:0] dc; //read data 3
	logic [31:0] RF_WriteData; // write data
	logic [4:0] RF_WriteAddr;
	logic [4:0] Read_addr1;
	logic [4:0] Read_addr2;
	logic [4:0] Read_addr3;
	////
	
/////////////////// PC logic /////////////////////////////

	
	always_comb begin 	/// PC = PC + 4
	PC_plus_4 = PC + 4;
	end
	
	//// PC Stage ////
	always@(posedge clock)begin
	
	if(reset) PC <= PCSTART;
	
	else if(stall || memory_stall) begin
		;
		end
    else begin
			// if(miss_predict) PC <= BHT_pc;
	    PC <= (PCSrc)? TargetAddress:PC_plus_4;  // (condition)? value_if_true : value_if_false;
		end
	end

	/////
	

	//// PC Address Generator ////
	always_comb begin
	TargetAddress=0;
	if(Jump) 		 				TargetAddress = IfId.PCincremented + (Imm20<<2);
	else if(Jump_Register) 		    TargetAddress = (Imm15) + dc;
	else if(SAJ)    				TargetAddress = IfId.PCincremented + (Imm5<<2); 
	else if(Branch) 				TargetAddress = IfId.PCincremented + (Imm15<<2);
	end
	
	always_comb begin
	PCSrc=0;
	if(Jump || SAJ || Jump_Register) PCSrc = 1'b1;
	else if(Branch_equal && zeroflag) PCSrc = 1'b1;
	else if(Branch_not_equal && !zeroflag) PCSrc = 1'b1;
	else if(Branch_greater && greaterflag) PCSrc = 1'b1;
	else if(Branch_less && !(greaterflag || zeroflag) ) PCSrc = 1'b1;
	else PCSrc=0;
	end
	
	
	/////
	
	/// Comparetor Logic ///
	logic [31:0]comp_in1;
	logic [31:0]comp_in2;
	always_comb begin
	comp_in2=32'bx;
	comp_in1=32'bx;
		case(ForwardC)
		3'b000:begin
			comp_in1 = da;
			end
		3'b001:begin
			comp_in1 = ExMem.AluOut1;
			end
		3'b010:begin
			comp_in1 = memWB.AluOut1;
			end
		3'b011:begin
			comp_in1 = memWB.AluOut2; //MA
			end
		3'b100:begin
			comp_in1 = datamem_data; //MA
			end
		3'b101:begin
			comp_in1 = memWB.Mem_data; //MA
			end	
		default:;
		endcase
		
		case(ForwardD)
		3'b000:begin
			comp_in2 = dc;
			end
		3'b001:begin
			comp_in2 = ExMem.AluOut1;
			end
		3'b010:begin
			comp_in2 = memWB.AluOut1;
			end
		3'b011:begin
			comp_in2 = memWB.AluOut2; //MA
			end
		3'b100:begin
			comp_in2 = datamem_data; 
			end
		3'b101:begin
			comp_in2 = memWB.Mem_data; 
			end	
		default:;
		endcase
	
	end
	always_comb begin
	greaterflag=(comp_in2>comp_in1)? 1'b1:1'b0;
	zeroflag=(comp_in2==comp_in1)? 1'b1:1'b0;
	end
	/////
	

/////////////////////////////////////////////////////////



	
		assign Opcode = IfId.instruction[6:0];
		assign funct12_out = IfId.instruction[31:22];
		assign flush = PCSrc;
	
//////////////////////  IF/ID Stage ////////////////////
always@(posedge clock)begin 

		if(stall || memory_stall);
		
		else if(reset || flush ) begin // if(reset || flush || miss_predict) begin
		IfId.instruction<=0;
		IfId.PCincremented<=0;
		IfId.rs1<=0;
		IfId.rs2<=0;
		IfId.rs3<=0;
		IfId.RegisterRd <=0;
		
		end
		
		else begin
		IfId.instruction<=instmem_data;
		IfId.PCincremented<=PC_plus_4;
		
		IfId.rs1<=instmem_data[16:12];
		IfId.rs2<=instmem_data[21:17];
		IfId.rs3<=instmem_data[11:7];
		IfId.RegisterRd <= instmem_data[11:7];
		end
	
	end
////////////////////
	
	// Read Register file
	assign Read_addr1 = IfId.instruction[16:12]; // rs1 address
	assign Read_addr2 = IfId.instruction[21:17]; // rs2 address
	assign Read_addr3 = IfId.instruction[11:7];  // rd  address
	assign RF_WriteAddr = memWB.RegisterRd;
	
	//Read Register file $r0 is always 0
	 assign da = (IfId.instruction[16:12]!=0)? RF[Read_addr1]: 0; // x0 is hardware as 1'b0
	 assign db = (IfId.instruction[21:17]!=0)? RF[Read_addr2]: 0; // 
	 assign dc = (IfId.instruction[11:7]!=0)? RF[Read_addr3]: 0; // 

//////////////////////// Write to Register file 
// **Depending on ISA
	always_comb begin
	RF_WriteData = 0;
	case(memWB.MemToReg)
	2'b00: RF_WriteData = memWB.Mem_data;
	
	2'b01: RF_WriteData = memWB.AluOut1;
	
	2'b10: RF_WriteData = memWB.AluOut2;
	default: ;
	endcase
	end

	always @(negedge clock) begin
		case(memWB.RegWrite)
		3'b000: ;
		3'b001: begin // 32-bit register write
				RF[RF_WriteAddr] <= RF_WriteData; 
				end
				
		3'b010: begin // LTH, load two register with 16-bit signed number.
				RF[RF_WriteAddr]   <= 32'(signed'(RF_WriteData[15:0]));
				RF[RF_WriteAddr+1] <= 32'(signed'(RF_WriteData[31:16]));
				end
				
		3'b011: begin // lh , load half-word
				RF[RF_WriteAddr]   <= 32'(signed'(RF_WriteData[15:0]));
				end
		3'b100: begin // lb , load byte
				RF[RF_WriteAddr]   <= 32'(signed'(RF_WriteData[7:0]));
				end
		3'b101: begin // lhu , load unsigned half-word
				RF[RF_WriteAddr]   <= {16'b0,RF_WriteData[15:0]};
				end
		3'b110: begin // lbu , load unsigned byte
				RF[RF_WriteAddr]   <= {24'b0,RF_WriteData[7:0]};
				end
		3'b111: begin // jalr
				RF[RF_WriteAddr]   <= memWB.PCincremented;
				end		
				
		default: ;
		endcase
		
	end
	
///////////////

logic [31:0]Imm5; // For SAJ instruction
logic [31:0]Imm10; // For SAJ instruction
logic [31:0]Imm15; // For I-type
logic [31:0]Imm20; // For J-type

//////////////Imm Generator (Sign Extend) ///////
always_comb begin

		Imm5 = 32'(signed'(IfId.instruction[21:17])); // Offset of BTA for SAJ
		Imm10 = 32'(signed'(IfId.instruction[31:22])); // Offset of DMA for SAJ
		Imm15 = 32'(signed'(IfId.instruction[31:17])); // Imm15 sign extension for I-type, S-type
		Imm20 = 32'(signed'(IfId.instruction[31:12])); // J-type
end

////////////




//////////////////// ID/EX Stage ////////////////////
	always@ (posedge clock)begin  //
	
		if(reset) begin
		
		IdEx.R_data1 <= 0;
		IdEx.R_data2 <= 0 ;
		IdEx.R_data3 <= 0 ;
		IdEx.sign_ext_32 <= 0; 
		IdEx.RegisterRd <=0 ;
		IdEx.PCincremented <=0;
		
		IdEx.rs1 <=0 ;
		IdEx.rs2 <=0 ;
		IdEx.rs3 <=0 ;
	
		//Control Signals
		IdEx.MemToReg <= 0;
		IdEx.ALUSrc <= 0;
		IdEx.MemWrite <= 0;
		IdEx.MemRead <= 0;
		IdEx.ALUOp1 <= 0;
		IdEx.ALUOp2 <= 0;
		IdEx.RegWrite <= 0;
		end
		else if(insert_nope) begin
		
		//Control Signals
		IdEx.MemToReg <= 0;
		IdEx.ALUSrc <= 0;
		IdEx.MemWrite <= 0;
		IdEx.MemRead <= 0;
		IdEx.ALUOp1 <= 0;
		IdEx.ALUOp2 <= 0;
		IdEx.RegWrite <= 0;
		
		IdEx.rs1 <=0 ;   // Reset them because dont forward by mistake
		IdEx.rs2 <=0 ;
		IdEx.rs3 <=0 ;
		IdEx.RegisterRd <=0 ;
		
		end
		else if(memory_stall) begin
		;
		end
		else begin
		IdEx.RegisterRd <= IfId.RegisterRd; // DOUBLE-CHECKED
		IdEx.R_data1 <= da ;
		IdEx.R_data2 <= db ;
		IdEx.R_data3 <= dc ;

		
		IdEx.sign_ext_32<=(SAJ)? Imm10:Imm15;
		IdEx.PCincremented <=IfId.PCincremented;
	
		//Control Signals
		IdEx.MemToReg <= MemToReg;
		IdEx.ALUSrc   <= ALUSrc;
		IdEx.MemWrite <= MemWrite;
		IdEx.MemRead  <= MemRead;
		IdEx.ALUOp1   <= ALUOp1;
		IdEx.ALUOp2   <= ALUOp2;
		IdEx.RegWrite <=RegWrite;
		
		
		IdEx.rs1 <= IfId.rs1 ;
		IdEx.rs2 <= IfId.rs2 ;
		IdEx.rs3 <= IfId.rs3 ;
		
		end
	end
///////////////


/////////////////////////// ALU Block -1- ///////////////////////
	integer ALU1_in1;
	integer ALU1_in2;
	integer ALU1_Result;
	
	always_comb begin ////////Forwarding A-B MUX//////
	
	case(ForwardA)
	  2'b00: begin //   
                  ALU1_in1 = IdEx.R_data1;
                  end  
      2'b01: begin //   
				    ALU1_in1 = RF_WriteData ;
                  end  
      2'b10: begin //  
					ALU1_in1 = ExMem.AluOut1;
				end	
	  2'b11: begin //  
					ALU1_in1 = memWB.AluOut2;
				end	
	default:;
	endcase
		
	case(ForwardB)   
    2'b00: begin //   
                        ALU1_in2 = (IdEx.ALUSrc)? IdEx.sign_ext_32 : IdEx.R_data2; 
                  end  
    2'b01: begin //   
						ALU1_in2 = (IdEx.ALUSrc)? IdEx.sign_ext_32 : RF_WriteData ;    
                  end  
    2'b10: begin //  
						ALU1_in2 = (IdEx.ALUSrc)? IdEx.sign_ext_32 : ExMem.AluOut1;
                  end 
	2'b11: begin //  
					    ALU1_in2 = (IdEx.ALUSrc)? IdEx.sign_ext_32 : memWB.AluOut2;
				end	
	default:;		  
	endcase
	
	end
	//////////////////-End of MUX-//////////////////
    always_comb
    begin
	ALU1_Result=0;
        case(IdEx.ALUOp1)
        4'b0000: // Addition
           ALU1_Result = ALU1_in1 + ALU1_in2 ; 
        4'b0001: // Subtraction
           ALU1_Result = ALU1_in1 - ALU1_in2 ;
        4'b0010: // Multiplication
           ALU1_Result = ALU1_in1 * ALU1_in2;
        4'b0011: // Logical shift left
			  ALU1_Result = ALU1_in1 << ALU1_in2;
        4'b0100: // Logical shift right
           ALU1_Result = ALU1_in1 >> ALU1_in2;
        4'b0101: // Arithmetic shift right
			  ALU1_Result = ALU1_in1 >>> ALU1_in2;
        4'b0110://  Logical and 
           ALU1_Result = ALU1_in1 & ALU1_in2; 
        4'b0111: //  Logical or
           ALU1_Result = ALU1_in1 | ALU1_in2;
        4'b1000: //  Logical xor 
           ALU1_Result = ALU1_in1 ^ ALU1_in2;
			  
        default: ALU1_Result = ALU1_in1 + ALU1_in2 ; 
        endcase
    end
///////////////

/////////////////////////// ALU Block -2- ///////////////////////
	integer ALU2_in1;
	integer ALU2_in2;
	integer ALU2_Result;
	
	assign ALU2_in1 = ExMem.AluOut1;
	assign ALU2_in2 = ExMem.R_data3; 

    always_comb
    begin
	ALU2_Result=0;
        case(IdEx.ALUOp2)
        4'b0000: // Addition
           ALU2_Result = ALU2_in1 + ALU2_in2 ; 
         /*4'b0001: // Subtraction
           ALU1_Result = ALU1_in1 - ALU1_in2 ;
        4'b0010: // Multiplication
           ALU1_Result = ALU1_in1 * ALU1_in2;
		4'b0011: // Division
           ALU_Result = ALU1_in1/ALU1_in2;
        4'b0100: // Logical shift left
           ALU_Result = ALU1_in1<<1;
         4'b0101: // Logical shift right
           ALU_Result = ALU1_in1>>1;
         4'b0110: // Rotate left
           ALU_Result = {ALU1_in1[30:0],ALU1_in1[31]};
         4'b0111: // Rotate right
           ALU_Result = {ALU1_in1[31],ALU1_in1[30:1]};
          4'b1000: //  Logical and 
           ALU_Result = ALU1_in1 & ALU1_in2;
          4'b1001: //  Logical or
           ALU_Result = ALU1_in1 | ALU1_in2;
          4'b1010: //  Logical xor 
           ALU_Result = ALU1_in1 ^ ALU1_in2;
          4'b1011: //  Logical nor
           ALU_Result = ~(ALU1_in1 | ALU1_in2);
          4'b1100: // Logical nand 
           ALU_Result = ~(ALU1_in1 & ALU1_in2);
          4'b1101: // Logical xnor
           ALU_Result = ~(ALU1_in1 ^ ALU1_in2);
          4'b1110: // Greater comparison
           ALU_Result = (ALU1_in1>ALU1_in2)?1'b1:1'b0 ;
          4'b1111: // Equal comparison   
            zeroflag = (ALU1_in1==ALU1_in2)?1'b1:1'b0 ;*/
          default: ALU2_Result = ALU2_in1 + ALU2_in2 ; 
        endcase
    end
///////////////
	
////////////////  EX/MEM Stage ////////////////////
	always@ (posedge clock)begin // 

		if(reset) begin // stall?
		ExMem.RegisterRd <= 5'b00000;
		ExMem.R_data3 <= 0;
		ExMem.R_data2 <= 0;
		ExMem.AluOut1 <= 0;
		// Control Signals
		ExMem.MemToReg <= 0;
		ExMem.MemWrite <= 0;
		ExMem.MemRead  <= 0;
		ExMem.RegWrite <= 0;
		ExMem.PCincremented <=0;
		end	
		else if(memory_stall) begin
		;
		end
		else begin
		ExMem.R_data2 <= IdEx.R_data2;
					case(ForwardE) // SW
		2'b00: begin //   
					ExMem.R_data3 <= IdEx.R_data3 ; //default memWB.AluOut2; ExMem.AluOut1; RF_WriteData
                  end  
		2'b01: begin //   
				    ExMem.R_data3 <= RF_WriteData; // memWB
                  end  
		2'b10: begin //  
					ExMem.R_data3 <= ExMem.AluOut1; // exmem
				end	
		2'b11: begin //  
					ExMem.R_data3 <= memWB.AluOut2; //memWB,ma
		end	
		default:;
		endcase
		ExMem.AluOut1 <= ALU1_Result;
		ExMem.RegisterRd <= IdEx.RegisterRd;
		ExMem.PCincremented <=IdEx.PCincremented;
		
		// Control Signals
		ExMem.MemToReg <= IdEx.MemToReg;
		ExMem.MemWrite <= IdEx.MemWrite;
		ExMem.MemRead  <= IdEx.MemRead;
		ExMem.RegWrite <= IdEx.RegWrite;
		
		ExMem.rs1 <= IdEx.rs1 ;
		ExMem.rs2 <= IdEx.rs2 ;
		ExMem.rs3 <= IdEx.rs3 ;
		
		end
	end
	

	
	
	
///////////////  MEM/WB Stage ////////////////////////
	always@ (posedge clock)begin
	if(reset) begin
	
	memWB.AluOut1 		<= 0;
	memWB.AluOut2 		<= 0;
	memWB.Mem_data 		<= 0;
	memWB.RegisterRd 	<= 0;
	memWB.PCincremented <= 0;
	end
	else if(memory_stall) begin
		;
	end
	else begin
	memWB.AluOut1 		<= ExMem.AluOut1;
	memWB.Mem_data 		<= datamem_data;
	memWB.RegisterRd 	<= ExMem.RegisterRd;
	memWB.AluOut2 		<= ALU2_Result;
	memWB.PCincremented <= ExMem.PCincremented;
	
	//Control Signals
	memWB.MemToReg <= ExMem.MemToReg;
	memWB.RegWrite <=ExMem.RegWrite;
	memWB.MemWrite <= ExMem.MemWrite;
	
		memWB.rs1 <= ExMem.rs1 ;
		memWB.rs2 <= ExMem.rs2 ;
		memWB.rs3 <= ExMem.rs3 ;

	
	end
	end
///////////////



// rs = rs1 rt=rs2
//////// HAZARD Detection Units  //////// I-type = rt destination,rs  R-type = rd destination,rt,rs
// 
logic [1:0]ForwardA ;
logic [1:0]ForwardB ;
logic [2:0]ForwardC ;
logic [2:0]ForwardD ;
logic [1:0]ForwardE ;

always_comb begin // Load-Use Hazard Detection Unit

		stall = 1'b0;
		insert_nope = 1'b0;

	if ( IdEx.MemRead && ( (IdEx.RegisterRd == IfId.rs1) || (IdEx.RegisterRd == IfId.rs2) || (IdEx.RegisterRd == IfId.rs3)) ) begin
		stall = 1'b1;
		insert_nope = 1'b1;
	end
	if ( (IdEx.MemToReg == 2'b10) && ((IdEx.RegisterRd == IfId.rs1) || (IdEx.RegisterRd == IfId.rs2)|| (IdEx.RegisterRd == IfId.rs3)) )begin
	    stall = 1'b1;
		insert_nope = 1'b1;
	end
	

end

always_comb begin // Data-Forwarding Unit
	ForwardA = 2'b00;
	ForwardB = 2'b00;
	ForwardE= 2'b00;
	
	if ((memWB.RegWrite!=0) && (memWB.RegisterRd != 0) && (ExMem.RegisterRd != IdEx.rs1) && (memWB.RegisterRd == IdEx.rs1)) begin
		ForwardA = 2'b01;   	// 
	end
 
	if ((memWB.RegWrite!=0) && (memWB.RegisterRd != 0) && (ExMem.RegisterRd != IdEx.rs2) && (memWB.RegisterRd == IdEx.rs2)) begin
		ForwardB = 2'b01;  	// 
	end
    if ( ( (IdEx.MemWrite != 0)||(IdEx.MemToReg == 2'b10) ) &&(memWB.RegWrite!=0) && (memWB.RegisterRd != 0) && (ExMem.RegisterRd != IdEx.rs3) && (memWB.RegisterRd == IdEx.rs3)) begin //SW
		ForwardE = 2'b01;  	// 
	end
	
	if ((ExMem.RegWrite!=0) && (ExMem.RegisterRd != 0) && (ExMem.RegisterRd == IdEx.rs1))begin
		ForwardA = 2'b10; //
	end
	
	if ((ExMem.RegWrite!=0) && (ExMem.RegisterRd != 0) && (ExMem.RegisterRd == IdEx.rs2))begin
		ForwardB = 2'b10; // 
	end
	
	if (( (IdEx.MemWrite != 0)||(IdEx.MemToReg == 2'b10) ) && (ExMem.RegWrite!=0) && (ExMem.RegisterRd != 0) && (ExMem.RegisterRd == IdEx.rs3))begin // SW
		ForwardE = 2'b10; // 
	end
	
	//Data forwarding for MA instruction
	if ((memWB.MemToReg==2'b10) && (memWB.RegisterRd != 0) && (ExMem.RegisterRd != IdEx.rs1) && (memWB.RegisterRd == IdEx.rs1)) begin
		ForwardA = 2'b11;   	// 
	end
 
	if ((memWB.MemToReg==2'b10) && (memWB.RegisterRd != 0) && (ExMem.RegisterRd != IdEx.rs2) && (memWB.RegisterRd == IdEx.rs2)) begin
		ForwardB = 2'b11;  	// 
	end
	
	if (( (IdEx.MemWrite != 0)||(IdEx.MemToReg == 2'b10) ) && (memWB.MemToReg==2'b10) && (memWB.RegisterRd != 0) && (ExMem.RegisterRd != IdEx.rs2) && (memWB.RegisterRd == IdEx.rs3)) begin //SW
		ForwardE = 2'b11;  	// 
	end


end

logic Branch;
assign Branch = (Branch_equal || Branch_greater || Branch_less || Branch_not_equal)? 1'b1:1'b0;
always_comb begin 
	ForwardC = 0;
	ForwardD = 0;
	
	if ( Branch && (ExMem.RegisterRd != 0) && (ExMem.RegisterRd == IfId.rs1) && ExMem.MemWrite==0 )begin // from exmem 
		ForwardC = 3'b001 ;
		end
	if ( Branch  && (ExMem.RegisterRd != 0) && (ExMem.RegisterRd == IfId.rs3) && ExMem.MemWrite==0	)begin // from exmem
		ForwardD = 3'b001 ;
		end
	if ( Branch &&  (ExMem.RegisterRd != 0) && (ExMem.RegisterRd != memWB.RegisterRd ) && (memWB.RegisterRd == IfId.rs1) && memWB.MemWrite==0 )begin// from memwb
		ForwardC = 3'b010 ;
		end
	if ( Branch  && (ExMem.RegisterRd != 0) && (ExMem.RegisterRd != memWB.RegisterRd ) && (memWB.RegisterRd == IfId.rs3) && memWB.MemWrite==0 )begin // from memwb
		ForwardD = 3'b010 ;
		end
	if ((memWB.MemToReg==2'b10) && Branch &&  (ExMem.RegisterRd != 0) && (ExMem.RegisterRd != memWB.RegisterRd ) && (memWB.RegisterRd == IfId.rs1) && memWB.MemWrite==0 )begin // MA
		ForwardC = 3'b011 ;
		end
	if ((memWB.MemToReg==2'b10) && Branch  && (ExMem.RegisterRd != 0) && (ExMem.RegisterRd != memWB.RegisterRd ) && (memWB.RegisterRd == IfId.rs3) && memWB.MemWrite==0 )begin // MA
		ForwardD = 3'b011 ;
		end
		
	if ((ExMem.MemRead) && Branch &&  (ExMem.RegisterRd != 0) && (ExMem.RegisterRd == IfId.rs1)  )begin // 
		ForwardC = 3'b100 ;
		end
	if ((ExMem.MemRead) && Branch  && (ExMem.RegisterRd != 0) && (ExMem.RegisterRd == IfId.rs3) )begin // 
		ForwardD = 3'b100 ;
		end
	if ((ExMem.MemRead) && Branch &&  (ExMem.RegisterRd != 0) && (ExMem.RegisterRd != memWB.RegisterRd ) && (memWB.RegisterRd == IfId.rs1) )  begin // 
		ForwardC = 3'b101 ;
		end
	if ((ExMem.MemRead) && Branch  && (ExMem.RegisterRd != 0) && (ExMem.RegisterRd != memWB.RegisterRd ) && (memWB.RegisterRd == IfId.rs3) ) begin // 
		ForwardD = 3'b101 ;
		end
		
end
///////////////////////////////////


endmodule
