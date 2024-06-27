#include "mmu.h"

#include <stdio.h>

bool canOnlyReplaceBlock(Line);

int memoryCacheMapping(int, Cache *);

int lineWhichWillLeave(int, Cache *);

void updateMachineInfos(Machine *, WhereWasHit *, int);

void moveLine();

char *convertToString(WhereWasHit whereWasHit)
{
    switch (whereWasHit)
    {
    case L1Hit:
        return "CL1";
    case L2Hit:
        return "CL2";
    case L3Hit:
        return "CL3";
    case RAMHit:
        return "RAM";
    default:
        return "";
    }
}

Line *MMUSearchOnMemorys(Address add, Machine *machine, WhereWasHit *whereWasHit)
{
    int l1pos = memoryCacheMapping(add.block, &machine->l1);
    int l2pos = memoryCacheMapping(add.block, &machine->l2);
    int l3pos = memoryCacheMapping(add.block, &machine->l3);

    Line *cache1 = machine->l1.lines;
    Line *cache2 = machine->l2.lines;
    Line *cache3 = machine->l3.lines;

    MemoryBlock *RAM = machine->ram.blocks;
    int cost = 0;

    // [BLOCO NA L1]
    if (cache1[l1pos].tag == add.block)
    {
        /* Block is in memory cache L1 */
        cost = COST_ACCESS_L1;
        *whereWasHit = L1Hit;
    }

    // [BLOCO NA L2]
    else if (cache2[l2pos].tag == add.block)
    {
        /* Block is in memory cache L2 */
        cache2[l2pos].tag = add.block;
        cost = COST_ACCESS_L1 + COST_ACCESS_L2;
        *whereWasHit = L2Hit;
        {
            // tinha um tmp aqui, mas não serviu p/ nada..

            if (!canOnlyReplaceBlock(cache1[l1pos]))
            {
                if (!canOnlyReplaceBlock(cache2[l2pos]))
                {
                    cache3[l3pos] = cache2[l2pos];
                }
                else
                {
                    cache2[l2pos] = cache1[l1pos];
                }
                cache2[l2pos] = cache1[l1pos];
            }
        }
    }
    // [BLOCO NA L3]
    else if (cache3[l3pos].tag == add.block)
    {

        cache3[l3pos].tag = add.block;

        cost = COST_ACCESS_L1 + COST_ACCESS_L2 + COST_ACCESS_L3;
        *whereWasHit = L3Hit;

        // [SEÇÃO DE BACKUP]
        if (!canOnlyReplaceBlock(cache1[l1pos]))
        { // se nao puder substituir -> backup
            if (!canOnlyReplaceBlock(cache2[l2pos]))
            {
                if (!canOnlyReplaceBlock(cache3[l3pos]))
                {                                                 // se nao puder colocar em l3, joga p/ ram;
                    RAM[cache3[l3pos].tag] = cache3[l3pos].block; // bloco diferente de linha, substituo so o bloco aq entao;
                }
                else
                { // se puder colocar em l3, coloca;
                    cache3[l3pos] = cache2[l2pos];
                }
                cache3[l3pos] = cache2[l2pos]; // backup de l2 em l3;
            }
            else
            { // se puder colocar em l2, coloca;
                cache2[l2pos] = cache1[l1pos];
            }
            cache2[l2pos] = cache1[l1pos]; // backup de l2 em l1;
        }

        updateMachineInfos(machine, whereWasHit, cost);
        return &(cache3[l3pos]); // achou entao ja retorna q ta em l3;
    }

    // [BLOCO NA RAM]
    else
    {
        /* Block only in memory RAM, need to bring it to cache and manipulate the blocks */
        int l2pos = lineWhichWillLeave(cache1[l1pos].tag, &machine->l2); /* Need to check the position of the block that will leave the L1 */
        int l3pos = lineWhichWillLeave(cache1[l1pos].tag, &machine->l3);

        if (!canOnlyReplaceBlock(cache1[l1pos]))
        {
            /* The block on cache L1 cannot only be replaced, the memories must be updated */
            if (!canOnlyReplaceBlock(cache2[l2pos]))
            {
                /* The block on cache L2 cannot only be replaced, the memories must be updated */
                if (!canOnlyReplaceBlock(cache3[l3pos]))
                {
                    // The block on cache L3 cannot only be replaced, the memories must be updated //
                    RAM[cache3[l3pos].tag] = cache3[l3pos].block;
                }
                else
                { // posso substituir l3
                    cache3[l3pos] = cache2[l2pos];
                }
                cache3[l3pos] = cache2[l2pos];
            }
            else
            {
                cache2[l2pos] = cache1[l1pos];
            }
            cache2[l2pos] = cache1[l1pos]; // se entrar no if, tem q passar por esse estagio aqui ;
        }

        cache1[l1pos].block = RAM[add.block];
        cache1[l1pos].tag = add.block;
        cache1[l1pos].updated = false;
        cost = COST_ACCESS_L1 + COST_ACCESS_L2 + COST_ACCESS_RAM;
        *whereWasHit = RAMHit;
        updateMachineInfos(machine, whereWasHit, cost);
        return &(cache1[l1pos]);
    }

    return &(cache1[l1pos]);
}

bool canOnlyReplaceBlock(Line line)
{
    // Or the block is empty or
    // the block is equal to the one in memory RAM and can be replaced
    if (line.tag == INVALID_ADD || (line.tag != INVALID_ADD && !line.updated)) // se n for endereço falso, mas estiver vermelho
        return true;
    return false;
}

int memoryCacheMapping(int address, Cache *cache)
{
    // [RETORNA A POSIÇÃO DO BLOCO ALVEJADO]
    for (int i = 0; i < cache->size; i++)
        if (address == cache->lines[i].tag)
            return i;
    // caso nao ache, retorna a menos usada;
    return lineWhichWillLeave(address, cache);
}

int lineWhichWillLeave(int address, Cache *cache)
{
    int pos = 0;
    // [CASO HAJA POSIÇÕES VAGAS]
    for (int i = 0; i < cache->size; i++)
    {
        if (cache->lines[i].tag == INVALID_ADD) // -1
            return i;
    }

#ifdef LFU
    // [LFU]
    int menor = cache->lines[0].count;
    for (int i = 1; i < cache->size; i++)
    {
        if (menor > cache->lines[i].count)
        {
            pos = i;
            menor = cache->lines[i].count;
        }
    }
#endif

    /*
    #ifdef LRU
        for (int i = 0; i < cache->size; i++)
        {
            if ()
        }

    #endif*/

    return pos;
}

void updateMachineInfos(Machine *machine, WhereWasHit *whereWasHit, int cost)
{
    switch (*whereWasHit)
    {
    case L1Hit:
        machine->hitL1 += 1;
        break;

    case L2Hit:
        machine->hitL2 += 1;
        machine->missL1 += 1;
        break;

    case L3Hit:
        machine->hitL3++;
        machine->missL1++;
        machine->missL2++;
        break;

    case RAMHit:
        machine->hitRAM += 1;
        machine->missL1 += 1;
        machine->missL2 += 1;
        break;
    }
    machine->totalCost += cost;
}