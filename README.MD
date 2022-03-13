# BIC Compiler

## Overview
Basic Instruction Compiler. The idea is to present an intermediate language that both captures the simplistic nature of assembly, but also offers a level of abstraction that both is straightforward to compile to and straightforward to work with.

Essentially this exists to act as both a target language, and source language for compiler projects.

## Compiling
To compile, run the following under a unix based system. Originally developed on OSX with CMake.
```
./configure.sh
./build.sh
```

To test, run: 
```
./configure.sh
./test.sh
```

## Architecture

### Registers
Includes support for GP-x64 registers only.

### Syntax
```
123		- Numerical values prefixed with $
\*rax	- Dereferencing pointer values prefixed with \* (i.e. deref rax)
lbl		- Label. No special syntax
\# rem	- Comments prefixed with \#
{typ} x - Local variable (i.e. int result) declaration
```

### Instruction set
```
ADD {reg64a}, {reg64b} 	- {reg64a} + {reg64b}
SUB {reg64a}, {reg64b} 	- {reg64a} - {reg64b}
MUL {reg64a}, {reg64b} 	- {reg64a} * {reg64b}
DIV {reg64a}, {reg64b} 	- {reg64a} / {reg64b}

MOV	{reg64a}, {reg64b}	- {reg64a} := {reg64b}

CALL {label}			- Branch to {label}, return to address
JMP {label}				- Branch to {label}
```

### Extensions/Abstractions

#### Procedures
```
proc {proc\_name}:
	int {arg1}, int {arg2}
begin
	int result

	mov result, 1
	add result, 2

	mov rax, result

	return rax;
endproc
```

#### If statements
if ax < bx:
endif
