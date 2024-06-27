#ifndef GENERATOR_H
#define GENERATOR_H

#include "cpu.h"

Instruction* generateRandomInstructions(int);
Instruction* readInstructions(char*, int*);
Instruction* generateMultiInstructions(int, int);
Instruction* generateDiviInstructions(int, int);

#endif // !GENERATOR_H