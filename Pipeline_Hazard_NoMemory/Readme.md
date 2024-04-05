# M2Y CPU Implementation
This folder contains the datapath and control unit of the processor with a hazard detection unit, but no memory hierarchy. No memory hierarchy means that memory is assumed as a directly accessible block.
<img width="768" alt="datapath_diagram" src="https://github.com/memreduman/Computer-Architecture/assets/60675917/c4c92737-8453-4259-8a3d-f46eabfcc28b">
## Design decisions:
The CPU exclusively supports relative-addressing mode, resulting in a shorter address field within instructions. This decision not only streamlines the instruction format but also simplifies the Datapath by eliminating the need for a Direct addressing logic unit.
Calculations for Branch and Jump instructions occur during the decode stage, significantly reducing the complexity of flush logic and minimizing lost cycles during branching.
The register file allows for simultaneous reading of three registers, enhancing the flexibility of instruction formatting. However, this does introduce complexity to the forwarding unit.
To address situations where data forwarding is not possible, the Datapath incorporates a nope inserter, which inserts a nope instruction as necessary.
