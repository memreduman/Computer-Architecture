# Computer Architecture
This repository contains a graduate-level project which is designing an embedded microprocessor in SystemVerilog.
First of all, the design has its own Instruction Set Architecture(ISA) based on RISC-V. [ISA_M2Y_ver1.2.pdf](https://github.com/memreduman/Computer-Architecture/files/13467790/ISA_M2Y_ver1.2.pdf) 

## The Folders
### 1. [M2Y Assembler](https://github.com/memreduman/Computer-Architecture/tree/main/M2Y_Assembler#m2y_assembler)
It is developed an assembler for M2Y Instruction Set written in C++. The program takes assembly code as an input and produces hex code for the CPU's instruction memory.
### 2. [Pipeline + Hazard](https://github.com/memreduman/Computer-Architecture/tree/main/Pipeline_Hazard_NoMemory)
This folder describes the implementation of CPU architecture with hazard detection units. No memory hierarchy in this folder to understand design better.
### 3. [Pipeline + Hazard + Branch Prediction](https://github.com/memreduman/Computer-Architecture/tree/601435d0fce4a356e21fd74214581ac0db248a9a/pipeline_hazard_BTB)
This folder describes the implementation of 2-bit Dynamic Branch Prediction
### 4. [Pipeline + Memory](https://github.com/memreduman/Computer-Architecture/tree/main/Pipeline_Memory)
This folder describes the implementation of CPU architecture with hazard detection units and memory hierarchy.
I am still organizing this folder DO NOT USE!
