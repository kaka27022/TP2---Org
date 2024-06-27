#ifndef INSTRUCTION_H
#define INSTRUCTION_H

typedef struct
{
    //  [POSICOES]
    int block; // 0 a ramSize - 1
    int word;  // (0 a 3)
} Address;

typedef struct
{
    int opcode;
    Address add1;
    Address add2;
    Address add3;
} Instruction;

Address getAdd1(Instruction *instruction);
void setAdd1(Instruction *instruction, Address add);

Address getAdd2(Instruction *instruction);
void setAdd2(Instruction *instruction, Address add);

Address getAdd3(Instruction *instruction);
void setAdd3(Instruction *instruction, Address add);

int getOpcode(Instruction *instruction);
void setOpcode(Instruction *instruction, int opcode);

#endif // !INSTRUCTION_H