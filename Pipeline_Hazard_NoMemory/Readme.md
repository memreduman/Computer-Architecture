# M2Y CPU Implementation
This folder contains the datapath and control unit of the processor with a hazard detection unit, but no memory hierarchy. No memory hierarchy means that data and instruction memories are assumed to be a directly accessible block.
<img width="768" alt="datapath_diagram" src="https://github.com/memreduman/Computer-Architecture/assets/60675917/c4c92737-8453-4259-8a3d-f46eabfcc28b">
## Design decisions:
<p align="justify">
<li>The CPU exclusively supports relative-addressing mode, resulting in a shorter address field within instructions. This decision not only streamlines the instruction format but also simplifies the Datapath by eliminating the need for a Direct addressing logic unit.</br></li>
<li>Calculations for Branch and Jump instructions occur during the decode stage, significantly reducing the complexity of flush logic and minimizing lost cycles during branching.</br></li>
<li>The register file allows for simultaneous reading of three registers, enhancing the flexibility of instruction formatting. However, this does introduce complexity to the forwarding unit.</br></li>
<li>To address situations where data forwarding is not possible, the Datapath incorporates a nope inserter, which inserts a nope instruction as necessary.</br></li>
</p>

<h2>Testbench:</h2>

An assembly program to verify the functionality and performance of the CPU.

<img width="400" alt="IAXP_description" src="https://github.com/memreduman/Computer-Architecture/assets/60675917/34ed474f-7ae4-4f3d-8361-101b29931935">
<img height = "400" width="300" alt="IAXP_assembly" src="https://github.com/memreduman/Computer-Architecture/assets/60675917/3d8edb4d-3caf-4f88-8ed4-43c4456fb3f8">

The code on the left-hand side is provided as IAXPY_assembly. The assembly code is converted to object code by using [M2Y Assembler](https://github.com/memreduman/Computer-Architecture/tree/main/M2Y_Assembler#m2y_assembler)
<h3>Result:</h3>
<img width="400" alt="verilator_result" src="https://github.com/memreduman/Computer-Architecture/assets/60675917/0fb0aaa2-e899-4933-adf2-5e5bdfcd4b7b">

A verilator program is developed to carry out functionality tests of the CPU. Verilator can generate cycle-accurate models of digital designs, making it useful for system-level simulations and testing. The verilator is also provided as sim_main_IAXPY.cpp
