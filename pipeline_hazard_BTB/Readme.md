# Dynamic Branch Prediction Integration
<p align="justify">
Dynamic branch prediction is a technique used to improve the performance of pipelined processors. Dynamic branch prediction aims to predict the outcome of branch instructions before their actual outcomes are known, enabling the processor to continue fetching and executing instructions without waiting for the branch resolution. This prediction is based on past behavior or patterns observed during program execution
<a href="https://www.cs.umd.edu/~meesh/411/CA-online/chapter/dynamic-branch-prediction/index.html">Further information</a>.
 </p>

 ## Design decisions: Branch-Target Buffers
<b>Structure</b>

<img style="margin-right: 1.5rem" align="left" height="auto" width="400" alt="Branch_Target_Buffer_Structure" src="https://github.com/memreduman/Computer-Architecture/assets/60675917/abf71a8d-0631-46b6-8074-85290835f7a4">
<p align="justify" >A branch prediction buffer, aka branch history table (BHT), 
in the IF stage addressed by the lower bits of the PC, 
contains a bit passed to the ID stage through the IF/ID pipeline register that 
tells whether the branch was taken the last time it was executed.
<br>Branch decision occurs in the ID stage after determining that the fetched
instruction is a branch and checking the prediction bit.
  <br><br><br><br><br><br>
</p>

<p align="justify">
<b>Steps in handling instruction with a Branch-Target Buffer </b>
</p>
<img style="margin-right: 1.5rem" align="left" height="auto" width="600" alt="Branch_Target_Buffer_Flow" src="https://github.com/memreduman/Computer-Architecture/assets/60675917/c33609e1-225c-4b96-b902-869a484dbd48">
<p align="justify">
Prediction bit may predict incorrectly (may be a wrong prediction for this branch
this iteration or may be from a different branch with the same low order PC
bits) but this doesn’t affect correctness, just performance.
<br>If the prediction is wrong, flush the incorrect instruction(s) in pipeline, restart the
pipeline with the right instruction, and invert the prediction bit
</p>
 <br><br><br><br><br><br>
<h2>Testbench:</h2>

An assembly program to verify the functionality and performance of the CPU.

<img width="400" alt="IAXP_description" src="https://github.com/memreduman/Computer-Architecture/assets/60675917/34ed474f-7ae4-4f3d-8361-101b29931935">
<img height = "400" width="300" alt="IAXP_assembly" src="https://github.com/memreduman/Computer-Architecture/assets/60675917/3d8edb4d-3caf-4f88-8ed4-43c4456fb3f8">

The code on the left-hand side is provided as IAXPY_assembly. The assembly code is converted to object code by using [M2Y Assembler](https://github.com/memreduman/Computer-Architecture/tree/main/M2Y_Assembler#m2y_assembler)
<h3>Result:</h3>
