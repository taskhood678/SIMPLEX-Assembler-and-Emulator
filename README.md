# SIMPLEX Two-Pass Assembler And Emulator

A custom-built, two-pass assembler and fully functioning CPU emulator for the SIMPLEX architecture, written entirely in modern C++. This project demonstrates low-level system architecture, memory management, bitwise manipulation, and robust error handling.

## Key Features

### The Two-Pass Assembler ('asm.cpp')
* Forward Reference Resolution:  Utilizes a two-pass architecture to seamlessly handle forward-referenced labels and branching offsets.
* Extensive Error Detection: Catches duplicate labels, undefined references, missing operands, invalid numeric formats, and unknown opcodes.
* Machine Code Generation: Outputs strict 32-bit binary object files ('.obj') and formatted listing files ('.lst') for debugging.

### The Hardware Emulator (`emu.cpp`)
* Fetch-Decode-Execute Cycle: Accurately simulates the SIMPLEX instruction set across 4 internal registers (A, B, PC, SP) and 64KB of virtual RAM.
* Memory Protection (Segfault Prevention): Validates all memory read/write requests, gracefully catching negative addresses and out-of-bounds array access without crashing the host C++ environment.
* Undefined Behavior Prevention: Clamps bitwise shift operations ('shl', 'shr') to safe limits (0-31) to prevent C++ compiler panics.
* Automated Flight Recorder: Automatically generates '.log' files containing memory dumps, register states, and execution errors for automated testing and grading.

## 🛠️ Building the Project

This project includes a 'Makefile' for quick compilation using 'g++'.

```bash
# Compile both the assembler and emulator
make all

# Clean up generated object and listing files
make clean

##Usage:
1. Assembling a Program
Translate a SIMPLEX assembly file into machine code:

./asm
# Prompt: Enter the name of the assembly file to process (include .asm at the end): 
# Input: bubble.asm

Outputs: bubble.obj, bubble.lst, and bubble.log

2. Emulating a Program
Execute the compiled binary object file on the virtual machine:

./emu
# Prompt: Enter the name of the binary object file (including the .obj extension):
# Input: bubble.obj

Outputs: Terminal execution summary, memory dump (optional), and a persistent bubble.log.

Assembly Example: Bubble Sort

Included in this repository is bubble.asm, a custom implementation of the Bubble Sort algorithm written strictly in SIMPLEX assembly. It utilizes the Stack Pointer (SP) to create local variables in virtual memory and sorts a 10-element array in place using custom branching and swapping logic.

Author: Raghuram Sathapathy | Academic Project