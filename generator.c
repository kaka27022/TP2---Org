#include "generator.h"
#include <stdio.h>
#include <stdlib.h>

// [PARA INSTRUÇÕES NO MODO "RANDOM"]
Instruction *generateRandomInstructions(int ramSize)
{
    // 01|22|13|45 => this is an instruction
    // 02|33|12|01 => this is another instruction

    // 0 => opcode => move
    // 1 => opcode => sum
    // 2 => opcode => subtract
    //-1 => halt

    int n = 10;

    Instruction *instructions = (Instruction *)malloc((n + 1) * sizeof(Instruction));

    for (int i = 0; i < n; i++)
    {
        // [ENDEREÇOS SÃO POSIÇÕES - TANTO BLOCO QUANTO PALAVRA]
        instructions[i].add1.block = rand() % ramSize;
        instructions[i].add1.word = rand() % WORDS_SIZE;
        instructions[i].add2.block = rand() % ramSize;
        instructions[i].add2.word = rand() % WORDS_SIZE;
        instructions[i].add3.block = rand() % ramSize;
        instructions[i].add3.word = rand() % WORDS_SIZE;
        instructions[i].opcode = rand() % 3;
    }
    instructions[n].opcode = -1;

    return instructions;
}

Instruction* generateMultiInstructions(int multiplier, int multiplying){
    Instruction* instructions = (Instruction*) malloc((3 + multiplier) * sizeof(Instruction));
    //Três instruções extras
        //1 - Salvar o multiplier na memória
        //2- Colocando 0 na posição no resultado na RAM
    instructions[0].opcode = 0;
    instructions[0].add1.block = multiplying; //Conteúdo a ser salvo na RAM
    instructions[0].add2.word = 0; //Posição da RAM
    
    instructions[1].opcode = 0;
    instructions[1].add1.block = 0; //Coloca 0 (elemento neutro da soma)
    instructions[1].add2.word = 1; //Posição da RAM

    for (int i = 0; i < multiplier; i++){
        instructions[i+2].opcode = 1; //Opcode para soma
        instructions[i+2].add1.word = 0; //Posição do multiplying 
        instructions[i+2].add2.word = 1; //Posição do resultado da multiplicaçao
        instructions[i+2].add3.word = 1; //Posição do resultado da multiplicaçao
    }

    //Inserindo a última instrução do programa que não faz nada que presta
    instructions[multiplier+2].opcode = -1;
    instructions[multiplier+2].add1.block = -1;
    instructions[multiplier+2].add2.block = -1;
    instructions[multiplier+2].add3.block = -1;

    return instructions;
}

Instruction* generateDiviInstructions(int dividend, int divisor){


    Instruction* instructions = (Instruction*) malloc((4 + dividend/divisor) * sizeof(Instruction));

    instructions[0].opcode = 0;
    instructions[0].add1.block = dividend; //Conteúdo a ser salvo na RAM
    instructions[0].add2.word = 0; //Posição da RAM
    
    instructions[1].opcode = 0;
    instructions[1].add1.block = divisor; //Conteúdo a ser salvo na RAM
    instructions[1].add2.word = 1; //Posição da RAM


    int result = 0; // sempre somando 1 ao resultado, para saber quando parar, tambem serve como um indice para manter a contagem

    //loop enquanto der para dividir o número
    while ((result + 1) * divisor <= dividend){

        instructions[result + 2].opcode = 2; //Opcode para subtração
        instructions[result + 2].add1.word = 0; //Posição do dividendo
        instructions[result + 2].add2.word = 1; //Posição do divisor
        instructions[result + 2].add3.word = 0; //Posição do resultado da divisão
        result++;
    }


    //salvando o resultado na RAM
    instructions[result + 2].opcode = 0;
    instructions[result + 2].add1.block = result;    //Conteúdo a ser salvo na RAM
    instructions[result + 2].add2.word = 2;         //Posição da RAM



    //inserindo a ultima instrucao do programa que nao faz nada que presta

    instructions[result + 3].opcode = -1;
    instructions[result + 3].add1.block = -1;
    instructions[result + 3].add2.block = -1;
    instructions[result + 3].add3.block = -1;

    return instructions;
}



// [PARA INSTRUÇÕES NO MODO "FILE"]
Instruction *readInstructions(char *fileName, int *memoriesSize)
{
    Instruction *instructions = NULL;

    printf("FILE -> %s\n", fileName);
    FILE *file = fopen(fileName, "r"); // Open file in read mode

    if (file == NULL)
    {
        printf("Arquivo nao pode ser aberto.\n");
        exit(1);
    }

    int n, i = 0;
    fscanf(file, "%d %d", &n, &memoriesSize[0]);
    instructions = (Instruction *)malloc(n * sizeof(Instruction));
    while (i < n)
    {
        fscanf(file, "%d %d %d %d %d %d %d",
               &instructions[i].opcode,
               &instructions[i].add1.block, &instructions[i].add1.word,
               &instructions[i].add2.block, &instructions[i].add2.word,
               &instructions[i].add3.block, &instructions[i].add3.word);
        i++;
    }
    fclose(file); // Close file

    return instructions;
}
