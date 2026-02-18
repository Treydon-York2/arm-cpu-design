Status: Completed Academic Hardware Architecture Project
Environment: Segger Embedded Studio Simulation
Architecture Model: ARM Cortex-A7, Multi-Cycle (Non-Popelined)

ARM CPU Design Project

OVERVIEW:
This project involved the schematic-level design and validation of a simplified ARM-based CPU using a multi-cycle execution architecture. The system was implemented through structured datapath and control logic design and validated using ARM assembly programs executed in Segger Embedded Studio.

The objective was to translate computer architecture theory into a functional hardware model capable of executing arithmetic, logical, memory, and branch instructions across staged execution cycles.

SYSTEM ARCHITECTURE:
The CPU follows a multi-cycle architecture, where instructions are executed across multiple clock phases rather than a single-cycle implementation. Functional units are reused across cycles to reduce hardware redundancy, with execution sequencing governed by a centralized control unit.

The CPU design included -
* Arithmetic Logic Unit (ALU)
* Control Unit
* Instruction Register
* Data Memory
* Cache
* Multiplexers
* Register File

TECHNOLOGIES USED:
* ARM Assembly
* Segger Embedded Studio
* Altium Designer
* Digital Logic Schematic Modeling

DESIGN & IMPLEMENTATION:
The datapath was structured to support arithmetic operations, memory access, and branch instructions. Control signals were generated through a structured control unit to synchronize instruction decoding and execution stages.

CHALLENGES:
One key challenge was ensuring proper synchronization between control signals and data movement across subsystems. Instruction-level debugging was used to validate correct register updates and execution flow.

TESTING & VALIDATION:
Assembly test programs were written to verify arithmetic operations, memory reads/writes, and instruction sequencing. Debugging confirmed correct execution behavior across implemented instruction sets.

KEY TAKEAWAYS:
* Strengthened understanding of instruction pipelines and control logic
* Gained practical experience in hardware-level debugging
* Improved ability to translate theoretical architecture into functional design
