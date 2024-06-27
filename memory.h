#ifndef MEMORY_H
#define MEMORY_H

#include <stdlib.h>
#include <stdbool.h>

#include "constants.h"

// cada bloco tem 4 palavras;
// aqui tem os inteiros em si, as palavras;
typedef struct
{
    int words[WORDS_SIZE];
} MemoryBlock;

// a ram tem ramSize blocos e o tamanho;
typedef struct
{
    int size;
    MemoryBlock *blocks;
} RAM;

typedef struct
{
    int count; // para lfu e lru
    int tempo_ac;
    // bloco 5 vai p/ linha, que vai ocupar a pos. 5;
    int tag;           /* Address of the block in memory RAM */
    bool updated;      // alterou o conteúdo de alguma palavra?
    MemoryBlock block; // bloco em si, transferido p/ linha
} Line;

// tamanho da cache e todas as suas linhas;
typedef struct
{
    int size;
    Line *lines;
} Cache;

void startCache(Cache *, int);
void stopCache(Cache *);

void startRAM(RAM *, int);
void stopRAM(RAM *);

#endif // !MEMORY_H