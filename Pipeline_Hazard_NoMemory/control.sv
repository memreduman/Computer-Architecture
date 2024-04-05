module control (input logic [6:0] Opcode,
				input logic [9:0] funct12,
				input logic stall,
				output logic MemRead,ALUSrc,Jump,SAJ,Branch_equal,Branch_not_equal,Branch_greater,Branch_less,Jump_Register,
				output logic [1:0] MemWrite,
				output logic [1:0] MemToReg,
			    output logic [2:0] RegWrite,
				output logic [3:0] ALUOp1,
				output logic [3:0] ALUOp2,
				output logic [9:0]funct12_out_debug
				);

//assign shamt = funct12[4:0];

assign funct12_out_debug =funct12;


	always_comb // Sensivity list
	begin 
				  MemRead = 1'b0;  
				  MemWrite = 2'b00;  
				  MemToReg = 2'b00;  
				  RegWrite = 3'b000;
				  ALUSrc = 1'b0;   
				  Branch_equal= 1'b0;
				  Branch_not_equal = 1'b0;
				  Branch_greater= 1'b0;
				  Branch_less= 1'b0;
                  Jump = 1'b0;
				  Jump_Register = 1'b0;
				  SAJ = 1'b0;
                  ALUOp1 = 4'b0000;	
                  ALUOp2 = 4'b0000;	
      case(Opcode)   
	  // R TYPE INSTRUCTIONS 
	   7'b0000001: begin //R type
				RegWrite = 3'b001;
				ALUSrc = 1'b0;
				
			case(funct12[3:0])
				4'b0000: begin ALUOp1 = 4'b0000; MemToReg = 2'b01; end //ADD
				4'b0001: begin ALUOp1 = 4'b0001; MemToReg = 2'b01; end //SUB
				4'b0010: begin ALUOp1 = 4'b0010; MemToReg = 2'b01; end //MUL
				4'b0011: begin ALUOp1 = 4'b0011; MemToReg = 2'b01; end //SLL
				4'b0100: begin ALUOp1 = 4'b0100; MemToReg = 2'b01; end //SRL
				4'b0101: begin ALUOp1 = 4'b0101; MemToReg = 2'b01; end //SRA
				4'b0110: begin ALUOp1 = 4'b1000; MemToReg = 2'b01; end //XOR
				4'b0111: begin ALUOp1 = 4'b0110; MemToReg = 2'b01; end //AND
				4'b1000: begin ALUOp1 = 4'b0111; MemToReg = 2'b01; end  //OR
				4'b1001: begin // MA
						 ALUOp1 = 4'b0010;
						 ALUOp2 = 4'b0000;
						 MemToReg = 2'b10;  
						 end
				default: ;
				endcase
				end
				
				
	 // I TYPES INSTRUCTIONS
	   7'b0000010: begin // I-type  
				ALUSrc = 1'b1;
				RegWrite = 3'b001;	
			    // ADDI 
				ALUOp1 = 4'b0000; 
				MemToReg = 2'b01;
				end 
	  
	  7'b0000011: begin // I-type  
				ALUSrc = 1'b1;
				RegWrite = 3'b001;	
			    // SUBI
				ALUOp1 = 4'b0001; 
				MemToReg = 2'b01;
				end 

	  7'b0000100: begin // I-type  
				ALUSrc = 1'b1;
				RegWrite = 3'b001;	
			    // MULI 
				ALUOp1 = 4'b0010; 
				MemToReg = 2'b01;
				end 
	  
	   7'b0000101: begin // I-type  
				ALUSrc = 1'b1;
				RegWrite = 3'b001;	
			    // SLLI
				ALUOp1 = 4'b0011; 
				MemToReg = 2'b01;
				end 

	   7'b0000110: begin // I-type 
				ALUSrc = 1'b1;
				RegWrite = 3'b001;	
			    // SRLI
				ALUOp1 = 4'b0100;
				MemToReg = 2'b01;
				end 

	   7'b0000111: begin // I-type  
				ALUSrc = 1'b1;
				RegWrite = 3'b001;	
			    // SRAI
				ALUOp1 = 4'b0101; 
				MemToReg = 2'b01;
				end 

	    7'b0001000: begin // I-type 
				ALUSrc = 1'b1;
				RegWrite = 3'b001;	
			    // XORI
				ALUOp1 = 4'b1000; MemToReg = 2'b01;
				end 

	    7'b0001001: begin // I-type 
				ALUSrc = 1'b1;
				RegWrite = 3'b001;	
			    // ANDI
				ALUOp1 = 4'b0110; MemToReg = 2'b01;
				end 
	 
	    7'b0001010: begin // I-type  
				ALUSrc = 1'b1;
				RegWrite = 3'b001;	
			    // ORI
				ALUOp1 = 4'b0111; MemToReg = 2'b01;
				end 
	  
	  //Load Opertaion
	  // RegWrite = 000:nothing, (Done in datapath)
	  //            001:lw
	  //            010:LTH
	  //            011:lh
	  //            100:lb
	  //            101:lhu
	  //            110:lbu
	  //			111:jalr (rd = pc+4)
	 7'b0001011: begin    // load operations 
				MemRead = 1'b1;
				MemToReg = 2'b00; 
				ALUSrc = 1'b1; 
				ALUOp1 = 4'b0000;
				RegWrite = 3'b001;	 // lw			
                  end  			  
	 7'b0001100: begin // load operations 
				MemRead = 1'b1;
				MemToReg = 2'b00; 
				ALUSrc = 1'b1; 
				ALUOp1 = 4'b0000;
				RegWrite = 3'b011;// LH		 				
                  end  			  
				  
	7'b0001101: begin // load operations 
				MemRead = 1'b1;
				MemToReg = 2'b00; 
				ALUSrc = 1'b1; 
				ALUOp1 = 4'b0000;
				RegWrite = 3'b100;// LB		 				
                  end  				  
	7'b0001110: begin // load operations 
				MemRead = 1'b1;
				MemToReg = 2'b00; 
				ALUSrc = 1'b1; 
				ALUOp1 = 4'b0000;
				RegWrite = 3'b101;// LHU				
                  end  				  
	7'b0001111: begin // load operations 
				MemRead = 1'b1;
				MemToReg = 2'b00; 
				ALUSrc = 1'b1; 
				ALUOp1 = 4'b0000;
				RegWrite = 3'b110;// LBU				
                  end  
	7'b0010000: begin // load operations 
				MemRead = 1'b1;
				MemToReg = 2'b00; 
				ALUSrc = 1'b1; 
				ALUOp1 = 4'b0000;
				RegWrite = 3'b010;// LTH		
                  end 
	7'b0010001: begin // jalr , jump and link register
				Jump_Register   = 1'b1;
				//RegWrite = 3'b111;	//jalr
                  end 
	  
	  
	  
	  
	 // SB TYPE 
	 7'b0010010: begin // Branch operations  
				Branch_equal = 1'b1;     //beq
				end
	 7'b0010011: begin // Branch operations 
				Branch_not_equal = 1'b1; //bne
				end
	 7'b0010100: begin // Branch operations 
				Branch_greater = 1'b1;   //bge
				end
	 7'b0010101: begin // Branch operations 
				Branch_less  = 1'b1;     //blt	
				end

				
	// MemWrite = 00:nothing (Done in datapath)
	//            01:sw
	//            10:sh	
	//            11:sb	
	
	 7'b0010110: begin // Store operations  
				MemWrite = 2'b01; 
				ALUSrc = 1'b1;
				ALUOp1 = 4'b0000;//SW
                  end 		
	 7'b0011000: begin // Store operations  
				MemWrite = 2'b10; 
				ALUSrc = 1'b1;
				ALUOp1 = 4'b0000;//SH
                  end 
	 7'b0010111: begin // Store operations  
				MemWrite = 2'b11; 
				ALUSrc = 1'b1;
				ALUOp1 = 4'b0000;//SB
                  end 
	 7'b0011001: begin // Store operations  
				MemWrite = 2'b01; 
				ALUSrc = 1'b1;
				ALUOp1 = 4'b0000;//SAJ
				SAJ = 1'b1; 
                  end 	
	  
	  
	 // J TYPE
	 //jal
	 7'b0011010: begin   
				RegWrite = 3'b111;
				Jump = 1'b1;
                  end 	
	  
		default: ;
      endcase
	  end
    
endmodule
