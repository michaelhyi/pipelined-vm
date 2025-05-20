# Pipelined Virtual Machine

This project is a pipelined implementation of the LC-3 architecture.
The LC-3 architecture is 16-bit, 16-bit addressable, and contains an address
space of 2^16.

The processor is divided into 5 stages: `IF`, `ID`, `EX`, `MEM`, `WB`. Each
executes as a single thread to simulate parallelism. The virtual pipeline
supports pipeline stalls, read-after-write hazard handling, and control hazard
handling. Features that will be implemented in the future are branch prediction,
data forwarding, branch prediction, and interrupt handlers.

## Quick Start

Tools:
- C
- Make
- Bash
- Docker 

Requirements:
- Bash
- Docker

Run the Docker container:
```bash
./docker.sh # run `chmod +x ./docker.sh` if permission denied
```

To start the VM, run:
```bash
make ARGS="-l <assembled lc-3 obj file path>"
```

To test the VM, run:
```bash
make test
```

To debug the VM using gdb, run:
```bash
make debug 
make debug-tests # to run while testing
```

To run Valgrind on the VM, run:
```bash
make valgrind
```

To clean the project, run:
```bash
make clean 
```

To format the project, run:
```bash
make format 
```
