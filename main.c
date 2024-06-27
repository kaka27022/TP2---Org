#include "cpu.h"
#include "generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

int main(int argc, char **argv)
{

    srand(1507); // Inicializacao da semente para os numeros aleatorios.

    if (argc != 6)
    {
        printf("Numero de argumentos invalidos! Sao 6.\n");
        printf("Linha de execucao: ./exe TIPO_INSTRUCAO [TAMANHO_RAM|ARQUIVO_DE_INSTRUCOES] TAMANHO_L1 TAMANHO_L2\n");
        printf("\tExemplo 1 de execucao: ./exe random 10 2 4 4\n");
        printf("\tExemplo 2 de execucao: ./exe file arquivo_de_instrucoes.txt 2 4\n");
        return 0;
    }

    // -----------------
    // 0 -> RAM
    // 1 -> CACHE L1
    // 2 -> CACHE L2
    // 3 -> CACHE L3
    Machine machine;
    int memoriesSize[4];
    Instruction *instructions;

    memoriesSize[1] = atoi(argv[3]);
    memoriesSize[2] = atoi(argv[4]);
    memoriesSize[3] = atoi(argv[5]);

    if (!strcmp(argv[1], "random"))
    {
        memoriesSize[0] = atoi(argv[2]);
        instructions = generateRandomInstructions(memoriesSize[0]);
    }
    else if (!strcmp(argv[1], "file"))
    {
        instructions = readInstructions(argv[2], memoriesSize);
    }
    else if(strcmp(argv[1], "multiplicacao") == 0){
        memoriesSize[0] = atoi(argv[2]);
        instructions = generateMultiInstructions(rand() % 10 + 1, rand() % 10 + 1);
    } else if(strcmp(argv[1], "divisao") == 0){
        memoriesSize[0] = atoi(argv[2]);
        instructions = generateDiviInstructions(5, 100);
    } 
    else
    {
        printf("Invalid option.\n");
        return 0;
    }

    printf("Starting machine...\n");
    start(&machine, instructions, memoriesSize);
    if (memoriesSize[0] <= 10) // mudar p/ 100 dps
        printMemories(&machine);
    run(&machine);
    if (memoriesSize[0] <= 10) // mudar p/ 100 dps
        printMemories(&machine);
    stop(&machine);
    printf("Stopping machine...\n");
    return 0;
}