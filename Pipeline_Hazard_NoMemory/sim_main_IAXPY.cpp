// verilator simulation file: 
// EEE 446 Spring 2021
// Ali Muhtaroglu, Middle East Technical University - Northern Cyprus Campus
//
#include <stdio.h>
#include <verilated.h>
#include <verilated_vcd_c.h>
#include "testbench.h"
#include "Vtop.h"
#include "Vtop___024root.h" // Important for reading the internals

#include <iostream>
#include <bitset>
// Top level interface signals defined here:
#define funct12_debug	funct12_debug

// Internal signals defined here:
// Note systemverilog design hierarchy can be traced by appending __DOT__ at every level:
// tb->m_topsim->rootp->top__DOT__MemRead

#define Opcode         			top__DOT__Opcode
#define MemRead           		top__DOT__MemRead
#define MemWrite          		top__DOT__MemWrite
#define ALUSrc         			top__DOT__ALUSrc
#define	Branch_equal          	top__DOT__Branch_equal
#define	Branch_not_equal        top__DOT__Branch_not_equal
#define	Branch_greater          top__DOT__Branch_greater
#define	Branch_less             top__DOT__Branch_less
#define Jump           			top__DOT__Jump
#define Jump_Register           top__DOT__Jump_Register
#define SAJ           			top__DOT__SAJ
#define MemToReg           		top__DOT__MemToReg
#define RegWrite          		top__DOT__RegWrite
#define ALUOp1          		top__DOT__ALUOp1
#define ALUOp2          		top__DOT__ALUOp2
#define PC						top__DOT__u1__DOT__PC
#define IfId					top__DOT__u1__DOT__IfId
#define IdEx					top__DOT__u1__DOT__IdEx
#define ExMem					top__DOT__u1__DOT__ExMem
#define memWB					top__DOT__u1__DOT__memWB

#define RF						top__DOT__u1__DOT__RF
#define greaterflag 			top__DOT__u1__DOT__greaterflag
#define zeroflag 				top__DOT__u1__DOT__zeroflag
#define ForwardC 				top__DOT__u1__DOT__ForwardC
#define ForwardD				top__DOT__u1__DOT__ForwardD
#define ForwardE				top__DOT__u1__DOT__ForwardE
#define ForwardA				top__DOT__u1__DOT__ForwardA
#define ForwardB				top__DOT__u1__DOT__ForwardB
#define m_top					m_topsim->rootp
#define stall					top__DOT__stall
#define flush					top__DOT__u1__DOT__PCSrc

// In case you would like he simulator to do operations conditional to DEBUG mode:
#define DEBUG       0

using namespace std; // for the iostream(cpp)

int clock_count = 0; //Global variable for the clock counter

// Note the use of top level design name here after 'V' as class type:
class	TOPLEVEL_TB : public TESTBENCH<Vtop> {

  long m_tickcount;

public:

  TOPLEVEL_TB(void) {
  }

  // Every time this procedure is called, clock is ticked once and associated
  // simulation tests and/or signal outputs for debug can be put into action:
  void tick(void) { 

    TESTBENCH<Vtop>::tick();

    // we are often interested in keeping track of number of clock ticks:
    m_tickcount++;

    // if DEBUG macro is set, we will then dump some signal values out every clock cycle
    // only the top level interface signals are listed here, but note you can use the
    // above defined macros to look at internal signals as well in debug mode. This is
    // quite useful when gtkwave cannot handle very large memory arrays etc.
	
	if (DEBUG) {
	  printf("\n %ld Clock cycle\n",m_tickcount);
      printf("PC=%x\n",m_topsim->rootp->PC);
      printf("InstData=%08x\n",m_topsim->rootp->top__DOT__u1__DOT__instmem_data);
	  printf("RF Write Data=%d\n",m_topsim->rootp->top__DOT__u1__DOT__RF_WriteData);
	  printf("---Control Signals---");
	  printf("\nMemRead   = %d\tMemWrite  = %d\tALUSrc 	= %d\t\nSAJ= %d\tMemToReg = %d\tRegWrite = %d\tALUOp1 = %d\tALUOp2 = %d\t",m_top->MemRead,m_topsim->rootp->MemWrite,m_topsim->rootp->ALUSrc,m_topsim->rootp->SAJ,m_topsim->rootp->MemToReg,m_topsim->rootp->RegWrite,m_topsim->rootp->ALUOp1,m_topsim->rootp->ALUOp2);
	  printf("\nBranch_greater = %d"	,m_top->Branch_greater);
	  printf("\nBranch_equal = %d"		,m_top->Branch_equal);
	  printf("\nBranch_not_equal = %d"	,m_top->Branch_not_equal);
	  printf("\nBranch_less  = %d"		,m_top->Branch_less );
	  printf("\nzeroflag = %d"			,m_top->zeroflag);
	  printf("\ngreaterflag = %d"		,m_top->greaterflag);
	  printf("\nJump = %d"			,m_top->Jump);
	  printf("\nJump_Register = %d"		,m_top->Jump_Register);
	  printf("\nStall = %d"		,m_top->stall);
	  printf("\nFlush = %d"		,m_top->flush);
	  printf("\n\nForwardA = %x"		,m_top->ForwardA);
	  printf("\nForwardB = %x"		,m_top->ForwardB);
	  printf("\nForwardC = %x"		,m_top->ForwardC);
	  printf("\nForwardD = %x"		,m_top->ForwardD);
	  printf("\nForwardE = %x"		,m_top->ForwardE);
	  printf("\n-----------\n");

    }
  }
};


void display_RF(TOPLEVEL_TB *tb){
	
	 printf("\t--------------- Register Content Table ---------------\n");
		for(int i=0;i<32;i=i+4){
				for(int j=0;j<4;j++){
				printf("\tx%d=%u\t",j+i,tb->m_topsim->rootp->top__DOT__u1__DOT__RF[j+i]);
				}
		printf("\n");
		}	
	printf("\t-------------------------------------------------------\n");
}
void display_DataMem(TOPLEVEL_TB *tb){
	
	 printf("\t--------------- First 32 Data Memory Content Table ---------------\n");
		for(int i=0;i<32;i=i+4){
				for(int j=0;j<4;j++){
				printf(" MEM[%d]=",j+i);
				string binary = std::bitset<8>(tb->m_top->top__DOT__u1__DOT__datamem[j+i]).to_string(); 
				cout<<binary<<"\t";
				}

		printf("\n");
	
		}	
		printf("------ ----------------------- ------\n");
}


int main(int argc, char** argv, char** env){
	
  Verilated::commandArgs(argc, argv);
  // Create an instance of our module under test
  TOPLEVEL_TB *tb = new TOPLEVEL_TB;
  
  // Message to standard output that test is starting:
  printf("Executing the test ...\n");

  // Data will be dumped to trace file in gtkwave format to look at waveforms later:
  tb->opentrace("booth_waveforms.vcd");

  // Note this message will only be output if we are in DEBUG mode:
  if (DEBUG) printf("Giving the system 1 cycle to initialize with reset...\n");

  // Hit that reset button for one clock cycle:
  tb->m_topsim->reset = 1;
  tb->tick();
  tb->m_topsim->reset = 0;

  // Verify 
	int stall_cycle=0;
	int flush_counter=0;
  ////////////////////////////////// 

  
  //while(clock_count!=150)
  while(1) // 5-stage pipeline, so each instruction will be executed maximum within 5 clock-cycle
  {
	   tb->tick();
	   clock_count++;
	   if(tb->m_top->stall && !tb->m_top->flush)	stall_cycle++;
	   if(tb->m_top->flush)	flush_counter++;
	   if(tb->m_top->PC==180 && !tb->m_top->stall){
		   break;
	   }
  }
  
  display_RF(tb);
  display_DataMem(tb);
  printf("\n ---- Program Statistic ---- \n");
  printf("\n Stall Cycles = %d \n",stall_cycle);
  printf("\n Flush Counter = %d\n",flush_counter);
  printf("------ ----------------------- ------\n");
  int value[25];
  for(int i = 1; i <= 25 ; i++){
		  value[i-1] = i*100 + i*2;
		  //cout << value[i-1] << endl;
  }
  
  int mem_value = 0;
  int m_address = 128;
  int error_counter = 0;
  for(int i = 1; i <= 25 ; i++){
	mem_value |= ( tb->m_top->top__DOT__u1__DOT__datamem[m_address] << 24 ) | ( tb->m_top->top__DOT__u1__DOT__datamem[m_address+1] << 16 ) | ( tb->m_top->top__DOT__u1__DOT__datamem[m_address+2] << 8 ) | ( tb->m_top->top__DOT__u1__DOT__datamem[m_address+3]);
	printf("MEM[%d] : %d\n",m_address,mem_value);
	m_address += 4;
	if(mem_value != value[i-1]){
		error_counter++;
	}
	mem_value = 0;
  }
  printf("Error counter : \n%d\n",error_counter);


  exit(EXIT_SUCCESS);
  
}
