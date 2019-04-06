- used mimi.cs.mcgill.ca
- compiled using the following 2 commands: 
gcc -c cpu.c pcb.c ram.c kernel.c shell.c shellmemory.c interpreter.c memorymanager.c
gcc -o mykernel cpu.o pcb.o ram.o kernel.o shell.o shellmemory.o interpreter.o memorymanager.o