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

#define DRAM_BLOCK_SIZE           8
#define DCACHE_SIZE        16

#define CLOCK_F 175000000 // 175Mhz
#define Thermal_Power 208.99 // 208.99 mW
// Top level interface signals defined here:
#define funct12_debug	funct12_debug
#define memory_stall	memory_stall
#define opcode_debug	opcode_debug
#define damem_rw		damem_rw // d-cache_write_request->memory

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


#define icache_data_ready		top__DOT__u2__DOT__icache_data_to_cpu_ready
#define dcache_data_ready		top__DOT__u3__DOT__dcache_data_to_cpu_ready
#define dram_busy				top__DOT__dram_busy
#define dram_icache_array		top__DOT__u4__DOT__dram_interface_icache__DOT__dram__DOT__dram_array__DOT__mem
#define dram_dcache_array		top__DOT__u4__DOT__dram_interface_dcache__DOT__dram__DOT__dram_array__DOT__mem
#define dcache_valid			top__DOT__dcache_valid
#define dcache_rw				top__DOT__dcache_rw
//#define icache_valid			top__DOT__icache_valid
#define dcache_array			top__DOT__u3__DOT__data_cache_sram__DOT__dcache_sram
#define icache_array			top__DOT__u2__DOT__inst_cache_sram__DOT__icache_sram
// In case you would like he simulator to do operations conditional to DEBUG mode:
#define DEBUG       0

using namespace std; // for the iostream(cpp)

int clock_count = 0; //Global variable for the clock counter
int inst_memory_stall_count = 0;
int data_memory_stall_count = 0;
int data_write_stall_count = 0;
long total_clock_cycle=0;
int error_count=0;
int dram_busy_counter=0;
int executed_inst=0;
int mem_writeback=0;
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
	total_clock_cycle=m_tickcount;
	// && !m_top->reset
	if(!m_top->icache_data_ready && !m_top->stall) // Skip reset signal, count inst mem stall cycles
		inst_memory_stall_count++;
	if(!m_top->dcache_data_ready && m_top-> dcache_valid && !m_top->dcache_rw && !m_top->stall ) // Skip reset signal, count inst mem stall cycles
		data_memory_stall_count++;
	if(!m_top->dcache_data_ready && m_top-> dcache_valid && m_top->dcache_rw && !m_top->stall ) // Skip reset signal, count data mem read stall cycles
		data_write_stall_count++;
	if(m_top->damem_rw)
		mem_writeback++;
    // if DEBUG macro is set, we will then dump some signal values out every clock cycle
    // only the top level interface signals are listed here, but note you can use the
    // above defined macros to look at internal signals as well in debug mode. This is
    // quite useful when gtkwave cannot handle very large memory arrays etc.
	//&& (m_top->icache_data_ready || reset)
	// if (DEBUG)
	//if (DEBUG && m_top->icache_data_ready && m_top->dcache_data_ready && !m_top->dram_busy ) 
		
	if ( DEBUG && !(m_top->memory_stall) ){
	  printf("\n %ld Clock cycle\n",m_tickcount);
      printf("PC=%d\n",m_topsim->rootp->PC);
      printf("InstData=%x\n",m_topsim->rootp->top__DOT__u1__DOT__instmem_data);
	  printf("RF Write Data=%d\n",m_topsim->rootp->top__DOT__u1__DOT__RF_WriteData);
	  printf("---Control Signals---");
	  printf("\nMemRead   = %d\tMemWrite  = %d\tALUSrc 	= %d\t\nSAJ= %d\tMemToReg = %d\tRegWrite = %d\tALUOp1 = %d\tALUOp2 = %d\t",m_top->MemRead,m_top->MemWrite,m_topsim->rootp->ALUSrc,m_topsim->rootp->SAJ,m_topsim->rootp->MemToReg,m_topsim->rootp->RegWrite,m_topsim->rootp->ALUOp1,m_topsim->rootp->ALUOp2);
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
	
	  printf("\n\nInst Cache Data Ready = %d"		,m_top->icache_data_ready);
	  printf("\ndcache_rw =%d",m_top->dcache_rw);
	  printf("\nData Cache Data Ready = %d"		,m_top->dcache_data_ready);
	  //printf("\nInst Cache Valid = %d"		,m_top->icache_valid);
	  printf("\nData Cache Valid = %d"		,m_top->dcache_valid);
	  printf("\nDRAM Busy = %d"		,m_top->dram_busy);
	  printf("\nMemory Stall Signal = %d"		,m_top->memory_stall);
	  
	  
	  printf("\n-----------\n");

    }
	
	
	
  }
};


void display_RF(TOPLEVEL_TB *tb){
	
	 printf("\t--------------- Register Content Table ---------------\n");
		for(int i=0;i<32;i=i+4){
				for(int j=0;j<4;j++){
				printf("\tx%d=%d\t",j+i,tb->m_topsim->rootp->top__DOT__u1__DOT__RF[j+i]);
				}
		printf("\n");
		}	
	printf("\t-------------------------------------------------------\n");
}

void display_DRAM_inst(TOPLEVEL_TB *tb){
	
	 printf("\t--------------- First 32 DRAM Instruction Memory Content Table ---------------\n");
		for(int i=0;i<32;i=i+4){
				for(int j=0;j<4;j++){
				printf(" MEM[%d]=",j+i);
				string binary = std::bitset<8>(tb->m_top->dram_icache_array[j+i]).to_string(); //to binary 11111010000010100001111100010000
				cout<<binary<<"\t";
				}

		printf("\n");
	
		}	
		printf("------ ----------------------- ------\n");
}

void display_DRAM_data(TOPLEVEL_TB *tb){
	
	 printf("\t--------------- First 32 DRAM Data Memory Content Table ---------------\n");
		for(int i=2048;i<(2048+32);i=i+4){
				for(int j=0;j<4;j++){
				printf(" MEM[%d]=",j+i);
				string binary = std::bitset<8>(tb->m_top->dram_dcache_array[j+i]).to_string(); //to binary 11111010000010100001111100010000
				cout<<binary<<"\t";
				}

		printf("\n");
	
		}	
		printf("------ ----------------------- ------\n");
}

void display_dcache(TOPLEVEL_TB *tb){
	
	 	printf("\t--------------- Data Cache Content Table ---------------\n");
		
		for(int i=0;i<DCACHE_SIZE;i++){
			printf("Block-%.2d: ",i);
			for(int j=0;j<DRAM_BLOCK_SIZE;j++){
				printf("%d= %.7x\t ",j,tb->m_top->dcache_array[i][j] ); //m_top->dcache_array[block index 16][word index 8]
			}
			printf("\n");
		}
		printf("\n------ ----------------------- ------\n");
}

void display_icache(TOPLEVEL_TB *tb){
	
	 	printf("\t--------------- Instruction Cache Content Table ---------------\n");
		
		for(int i=0;i<8;i++){
			printf("Block-%.2d: ",i);
			for(int j=0;j<8;j++){
				printf("%d= %.7x\t  ",j,tb->m_top->icache_array[i][j] ); //m_top->dcache_array[block index 16][word index 8]
			}
			printf("\n");
		}
		printf("\n------ ----------------------- ------\n");
}

void functional_test(TOPLEVEL_TB *tb,int A){ //burda kaldım D cacheden değerleri çekmeye çalış
		uint32_t value = 0;
		uint32_t test_value = 0;
		for(int k=0;k<(A); k++){
			test_value = 10*k;
			
			for(int i=2048;i<(2048+A);i=i+4){
					int shift = 3;
					for(int j=0;j<4;j++){
						if(k>=128){
							value = value | (tb->m_top->dram_dcache_array[j+i] << shift*8);
						}
						else{
							value = value | (tb->m_top->dcache_array[0][0] << j*8);
						}
						cout << std::bitset<32>(value).to_string() << endl;
						shift--;
					}
			}
		
		}
		//printf("\n %d\n",value);
		
		/*
		for(int i=3;i<(1020);i=i+4){
			if(!(tb->m_top->dram_dcache_array[i]==*(a+(i/4)))){
				error_count++;
			}	
			
		}	
		*/
}





int main(int argc, char** argv, char** env){
	
	unsigned short n = 10;
	int A[n][n];
	int k=1;
	/// First loop for lab4
	for (int i=0 ; i < n ; i++) {
		for (int j=0 ; j < n ; j++) {
			A[i][j] = k;
			k++;
		}
	}
	
	/*
	unsigned short n = 10;
	int A[n][n];
	int k=1;
	/// First loop for lab4
	for (int i=0 ; i < n ; i++) {
		for (int j=0 ; j < n ; j++) {
			A[i][j] = k;
			k++;
		}
	}
     FILE *fp;
     char output[]="loop1.hex";
     fp=fopen(output,"w");
      
    for(int i=0;i<n;i++) {
		for (int j=0 ; j < n ; j++) {
			fprintf(fp,"%.8x",A[i][j]);
			fprintf(fp," ");
			fprintf(fp,"\n");
			
		}
    }
     fclose(fp);
	 
	/// Second loop for lab4
	
	for (int i=0 ; i < n ; i++) {
		A[i+1] = A[i];
	}
	
	 char output2[]="loop2.hex";
     fp=fopen(output2,"w");
      
    for(int i=0;i<n;i++) {
		fprintf(fp,"%.2x ",A[i]);
		if((i%4)==3)
		fprintf(fp,"\n");
    }
     fclose(fp);
	*/
	
  Verilated::commandArgs(argc, argv);
  // Create an instance of our module under test
  TOPLEVEL_TB *tb = new TOPLEVEL_TB;
  
  // Message to standard output that test is starting:
  printf("Executing the test ...\n");

  // Data will be dumped to trace file in gtkwave format to look at waveforms later:
  tb->opentrace("pipeline_memory.vcd");

  // Note this message will only be output if we are in DEBUG mode:
  if (DEBUG) printf("Giving the system 1 cycle to initialize with reset...\n");
  
  
  
  // Hit that reset button for one clock cycle:
  tb->m_topsim->reset = 1;
  tb->tick();
  clock_count++;
  tb->m_topsim->reset = 0;
  
  // Verify 

	int stall_cycle=0;
	int flush_counter=0;
	int m=0;
  ////////////////////////////////// 
  int N=2500; //without memory stall //1285
  while(clock_count<=N) // 5-stage pipeline, fill penalty = 4
  {
	   tb->tick();
	   clock_count++;
	   
	   while(tb->m_top->dram_busy){
		   tb->tick();
		   dram_busy_counter++;
	   }
	   
	   if(tb->m_top->opcode_debug>0 && !tb->m_top->stall) executed_inst++;
	   if(tb->m_top->stall && !tb->m_top->flush)	stall_cycle++;
	   if(tb->m_top->flush)	flush_counter++;
	   
	   if(tb->m_top->PC==13*4 && !tb->m_top->stall){ // Write stop address of program
	   while(m<5){
			   	   tb->tick();
				   clock_count++;   
			while(tb->m_top->dram_busy){
			tb->tick();
			dram_busy_counter++;
			}
				if(tb->m_top->stall && !tb->m_top->flush)	stall_cycle++;
				if(tb->m_top->flush)	flush_counter++;
		   m++;
		   }
		   break;
	   }
		
  }
		total_clock_cycle--;
		display_RF(tb);
		display_dcache(tb);
		//display_icache(tb);
		display_DRAM_data(tb);
		//display_DRAM_inst(tb);
		
		
		printf("\n ---- Program Statistic ---- \n");
		printf("\n Error count(Memory may not be updated yet) = %d\n",error_count);
		printf("\n Total Clock Cycle = %ld\n",total_clock_cycle);
		printf("\n Total Executed Inst = %d\n",executed_inst);
		printf("\n Stall Cycles(Load-Use) = %d\n",stall_cycle);
		printf("\n Flush Counter = %d\n",flush_counter);
		printf("\n Dram Busy Counter = %d\n",dram_busy_counter);
		printf("\n Inst. Memory Read Stall Counter = %d\n",inst_memory_stall_count);
		printf("\n Data  Memory Read Stall Counter = %d\n",data_memory_stall_count);
		printf("\n Data  Memory Write Stall Counter = %d\n",data_write_stall_count);
		printf("\n Inst. Memory Read Miss = %d\n",inst_memory_stall_count/37);
		printf("\n Data  Memory Read Miss = %d\n",data_memory_stall_count/37);
		printf("\n Data  Memory Write Miss = %d\n",((data_write_stall_count/37)-mem_writeback));
		printf("\n Data  Memory Write-Back Stall = %d\n"		,mem_writeback);
		float eff_cpi=(float)(total_clock_cycle-dram_busy_counter)/(executed_inst);
		float cpi = (float)(total_clock_cycle)/(executed_inst);
		printf("\n CPI without memory = %f\n",eff_cpi);
		printf("\n CPI with memory= %f\n",cpi);
		float exec_time = (float)((eff_cpi*(executed_inst))/CLOCK_F)*1000000; // in micro second
		float exec_time_mem = (float)((cpi*(executed_inst))/CLOCK_F)*1000000; // in micro second
		printf("\n Execution Time without memory hierarchy = %.3f us\n",exec_time);
		printf("\n Execution Time with memory hierarchy = %.3f us\n",exec_time_mem);
		
		printf("\n Energy consumption without memory hierarchy = %.3f uJ\n",exec_time*float(Thermal_Power*0.001));
		printf("\n Energy consumption with memory hierarchy = %.3f uJ\n",exec_time_mem*float(Thermal_Power*0.001));
		
		functional_test(tb,1);
		
  exit(EXIT_SUCCESS);
  
}
