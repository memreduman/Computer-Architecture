# M2Y_Assembler
This assembler is created for The RISC-V-M2Y Instruction Set Manual
[ISA_M2Y_ver1.2.pdf](https://github.com/memreduman/Computer-Architecture/files/13467790/ISA_M2Y_ver1.2.pdf) 
<p> M2Y Assembly code example </p>
<img width="428" alt="Screenshot 2023-11-29 022252" src="https://github.com/memreduman/Computer-Architecture/assets/60675917/385918bd-c03e-4bfc-84af-ef0d67348da5">
<p> <h3> How to use M2Y Assembler </h3> </p>
<p> The program asks for an input assembly code file and produces a HEX object code file named "input file name"_out. </p>
##Design decisions:
The CPU exclusively supports relative-addressing mode, resulting in a shorter address field within instructions. This decision not only streamlines the instruction format but also simplifies the Datapath by eliminating the need for a Direct addressing logic unit.
Calculations for Branch and Jump instructions occur during the decode stage, significantly reducing the complexity of flush logic and minimizing lost cycles during branching.
The register file allows for simultaneous reading of three registers, enhancing the flexibility of instruction formatting. However, this does introduce complexity to the forwarding unit.
To address situations where data forwarding is not possible, the Datapath incorporates a nope inserter, which inserts a nope instruction as necessary.
