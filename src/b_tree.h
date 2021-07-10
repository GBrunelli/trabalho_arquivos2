#ifndef B_TREE_H
#define B_TREE_H

#include "project.h"

#define ORDER 5
#define REGISTERS_PER_PAGE 4

typedef enum _Result
{
    PROMOTION = -100,
    NO_PROMOTION,
    FOUND,
    NOT_FOUND,
    ERROR
} Result;

typedef struct _Index        Index;
typedef struct _Register     Register;

/**
 * Creates and opens an empty index file, and set its status to inconsistent
*/
Index *createIndex(char *indexFileName);

/**
 * Opens an index file, and set its status to inconsistent
*/
Index *openIndex(char *indexFileName);

/**
 * Closes the index file, and set its status to consistent
*/
void *closeIndex(Index *index);

/**
 * Retuns an index register with key C and value Pr
*/
Register *createRegister(int32_t C, int64_t Pr);

/**
 * Insert a new register in the index file
*/
Result insertRegister(Index *index, Register *reg);

/**
 * Searches for a key in the index file,
 * if a register is founds, returns it 
*/
Result searchRegister(Index *index, int32_t key, Register **foundReg);

/**
 * Compare 2 **registers
*/
int compareRegisters(const void *a, const void *b);

#endif