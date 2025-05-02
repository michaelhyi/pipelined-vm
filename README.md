# Pipelined Virtual Machine

This project is a pipelined implementation of the LC-3 architecture.
The LC-3 architecture is 16-bit, 16-bit addressable, and contains an address
space of 2^16.

The processor is divided into 5 stages: `IF`, `ID`, `EX`, `MEM`, `WB`.
Features that will be implemented in the future are pipeline stalls, data
forwarding, conservative branch handling, branch prediction, and interrupt
handling.
