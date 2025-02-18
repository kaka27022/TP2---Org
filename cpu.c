#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "cpu.h"
#include "mmu.h"
#include "constants.h"

void start(Machine *machine, Instruction *instructions, int *memoriesSize)
{
        // memoriesSize[0] = ramSize
        startRAM(&machine->ram, memoriesSize[0]);
        startCache(&machine->l1, memoriesSize[1]);
        startCache(&machine->l2, memoriesSize[2]);
        startCache(&machine->l3, memoriesSize[3]);

        machine->instructions = instructions;

        machine->hitL1 = 0;
        machine->hitL2 = 0;
        machine->hitL3 = 0;
        machine->hitRAM = 0;

        machine->missL1 = 0;
        machine->missL2 = 0;
        machine->missL3 = 0;
        machine->totalCost = 0;
}

void stop(Machine *machine)
{
        free(machine->instructions);
        stopRAM(&machine->ram);
        stopCache(&machine->l1);
        stopCache(&machine->l2);
}

// [CORPO DO PROGRAMA]
void executeInstruction(Machine *machine, int PC)
{
        // [RECEBE A INSTRUÇÃO NA POSIÇÃO "PC" - N DA EXECUÇÃO ATUAL]
        Instruction instruction = machine->instructions[PC];

        // Registers
        int word1, word2;

        // Addresses to be consulted
        Address add1 = instruction.add1;
        Address add2 = instruction.add2;

        // ONDE O RESULTADO SERÁ SALVO ?
        Address add3 = instruction.add3;

        // Line find in memory
        Line *line;
        WhereWasHit hit;

        switch (instruction.opcode)
        {
        case -1:
                printf("  > Ending execution.\n");
                break;

        case 0: // Taking information to RAM
#ifdef PRINT_LOG
                printf("  > ATR (%4d) > ", add1.block);
#endif

                line = MMUSearchOnMemorys(add2, machine, &hit); /* Searching block on memories */
#ifdef PRINT_LOG
                printf("BLOCK[%s.%d.%d](%4d)\n", convertToString(hit), add2.block, add2.word, line->block.words[add2.word]);
#endif

                line->block.words[add2.word] = add1.block;
                line->updated = true;
                break;

                //  [SOMA]
        case 1:
                //      PRIMEIRO BLOCO

                line = MMUSearchOnMemorys(add1, machine, &hit); /* Searching block on memories */
                word1 = line->block.words[add1.word];

#ifdef PRINT_LOG
                printf("  > SUM BLOCK[%s.%d.%d](%4d)", convertToString(hit), add1.block, add1.word, line->block.words[add1.word]);
#endif
                //      + SEGUNDO BLOCO
                line = MMUSearchOnMemorys(add2, machine, &hit); /* Searching block on memories */
                word2 = line->block.words[add2.word];
#ifdef PRINT_LOG
                printf(" + BLOCK[%s.%d.%d](%4d)", convertToString(hit), add2.block, add2.word, line->block.words[add2.word]);
#endif

                line = MMUSearchOnMemorys(add3, machine, &hit); /* Searching block on memories */
#ifdef PRINT_LOG
                printf(" > BLOCK[%s.%d.%d](%4d|", convertToString(hit), add3.block, add3.word, line->block.words[add3.word]);
#endif

                line->updated = true;

                line->block.words[add3.word] = word2 + word1;
#ifdef PRINT_LOG
                printf("%4d).\n", line->block.words[add3.word]);
#endif
                break;

                //  [SUBTRAÇÃO]
        case 2:
                line = MMUSearchOnMemorys(add1, machine, &hit); /* Searching block on memories */
                word1 = line->block.words[add1.word];
#ifdef PRINT_LOG
                printf("  > SUB BLOCK[%s.%d.%d](%4d)", convertToString(hit), add1.block, add1.word, line->block.words[add1.word]);
#endif

                line = MMUSearchOnMemorys(add2, machine, &hit); /* Searching block on memories */
                word2 = line->block.words[add2.word];
#ifdef PRINT_LOG
                printf(" - BLOCK[%s.%d.%d](%4d)", convertToString(hit), add2.block, add2.word, line->block.words[add2.word]);
#endif

                line = MMUSearchOnMemorys(add3, machine, &hit); /* Searching block on memories */
#ifdef PRINT_LOG
                printf(" > BLOCK[%s.%d.%d](%4d|", convertToString(hit), add3.block, add3.word, line->block.words[add3.word]);
#endif

                line->updated = true;
                line->block.words[add3.word] = word2 - word1;

#ifdef PRINT_LOG
                printf("%4d).\n", line->block.words[add3.word]);
#endif

                break;

                //  [MOV]
        case 3:                                                 // Moving information in RAM
                line = MMUSearchOnMemorys(add1, machine, &hit); /* Searching block on memories */
                word1 = line->block.words[add1.word];
#ifdef PRINT_LOG
                printf("  > MOV BLOCK[%s.%d.%d](%4d) > ", convertToString(hit), add1.block, add1.word, line->block.words[add1.word]);
#endif

                line = MMUSearchOnMemorys(add2, machine, &hit); /* Searching block on memories */
#ifdef PRINT_LOG
                printf("BLOCK[%s.%d.%d](%4d|", convertToString(hit), add2.block, add2.word, line->block.words[add2.word]);
#endif

                line->block.words[add2.word] = word1;
                line->updated = true;
#ifdef PRINT_LOG
                printf("%4d).\n", line->block.words[add2.word]);
#endif
                break;

        default:
                printf("Invalid instruction.\n");
        }
#ifdef PRINT_INTERMEDIATE_RAM
        printMemories(machine);
#endif
}

void run(Machine *machine)
{
        int PC = 0; // numero de execucoes - uma instruct por vez;

        while (machine->instructions[PC].opcode != -1)
        {
                executeInstruction(machine, PC++);
                printf("\tL1:(%6d, %6d) | L2:(%6d, %6d) | L3:(%6d, %6d) | RAM:(%6d) | COST: %d\n",
                       machine->hitL1, machine->missL1,
                       machine->hitL2, machine->missL2,
                       machine->hitL3, machine->missL3,
                       machine->hitRAM,
                       machine->totalCost);
        }
}

void printc(char *text, int fieldWidth)
{
        int padlenLeft = floor((fieldWidth - strlen(text)) * 0.5);
        int padlenRight = ceil((fieldWidth - strlen(text)) * 0.5);
        printf("|%*s%s%*s|", padlenLeft, "", text, padlenRight, "");
}

void printcolored(int n, bool updated)
{
        if (updated)
                printf("\x1b[1;37;42m[%5d]\x1b[0m", n); // Green
        else
                printf("\x1b[1;37;41m[%5d]\x1b[0m", n); // Red
}

void printMemories(Machine *machine)
{
        printf("\x1b[0;30;47m      ");
        printc("RAM", WORDS_SIZE * 8 + 3);
        printc("Cache L3", WORDS_SIZE * 8 + 10);
        printc("Cache L2", WORDS_SIZE * 8 + 10);
        printc("Cache L1", WORDS_SIZE * 8 + 10);
        printf("\x1b[0m\n");
        for (int i = 0; i < machine->ram.size; i++)
        {
                printf("\x1b[0;30;47m%6d|\x1b[0m", i);
                for (int j = 0; j < WORDS_SIZE; j++)
                        printf(" %6d |", machine->ram.blocks[i].words[j]);
                if (i < machine->l3.size)
                {
                        printf("|");
                        printcolored(machine->l3.lines[i].tag, machine->l3.lines[i].updated);
                        for (int j = 0; j < WORDS_SIZE; j++)
                                printf(" %6d |", machine->l3.lines[i].block.words[j]);

                        if (i < machine->l2.size)
                        {
                                printf("|");
                                printcolored(machine->l2.lines[i].tag, machine->l2.lines[i].updated);
                                for (int j = 0; j < WORDS_SIZE; j++)
                                        printf(" %6d |", machine->l2.lines[i].block.words[j]);
                                if (i < machine->l1.size)
                                {
                                        printf("|");
                                        printcolored(machine->l1.lines[i].tag, machine->l1.lines[i].updated);
                                        for (int j = 0; j < WORDS_SIZE; j++)
                                                printf(" %6d |", machine->l1.lines[i].block.words[j]);
                                }
                        }
                }
                printf("\n");
        }
}
