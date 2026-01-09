## 5-Stage CPU Pipeline Simulator (C++)

# PIPELINE-SIMULATOR
This project is an interactive software simulation of a classic 5-stage RISC CPU pipeline, implemented in C++, designed to demonstrate core computer architecture concepts such as instruction pipelining, data hazards, and pipeline stalling.

# Overview
This project is a software simulation of a classic 5-stage RISC CPU pipeline:
- IF: Instruction Fetch
- ID: Instruction Decode
- EX: Execute
- MEM: Memory Access
- WB: Write Back

# Features
- Interactive instruction input
- User-defined memory initialization
- Cycle-by-cycle pipeline execution
- Load-use data hazard handling using pipeline stalling
- Multiple program runs without restarting

# Supported Instructions
ADD rd rs1 rs2
SUB rd rs1 rs2
LOAD rd address
STORE rs1 address
NOP

Registers: R0–R7

# Hazard Handling
- Load-use data hazards handled via pipeline stalls
- Forwarding discussed but not implemented (optimization)

# How to Run
g++ pipeline_simulator.cpp -o pipeline_simulator
./pipeline_simulator
