#include "project.h"
#include "line.h"
#include "car.h"
#include "utils.h"
#include "b_tree.h"

void indexLines()
{
    // Opening file names
    char binFileName[MAX_STRING_SIZE], indexFileName[MAX_STRING_SIZE];
    FILE *binFile = NULL, *indexFile = NULL;
    scanf("%s %s", binFileName, indexFileName);
    openFiles(&binFile, binFileName, "rb", &indexFile, indexFileName, "wb+", checkLineFileIntegrity);

    // Getting number of registers to be inserted
    int nreg = getLineTotalRegisters(binFile);

    // Creating containers
    Index *index = createIndex(indexFile);
    Line *l = newLine();
    Register *reg = newRegister();

    // Getting each line, then inserting its hash and offset into the index
    int offset = 0;
    for (int rrn = 0; rrn < nreg; rrn++)
    {
        offset = updateLine(l, binFile, BIN, NO_OFFSET);
        if (!lineLogicallyRemoved(l))
        {
            updateRegister(reg, getLineIndex(l), offset);
            insertRegister(index, reg);
        }
    }

    // Closing files
    freeLine(l);
    freeRegister(reg);
    closeIndex(index);
    fclose(binFile);

    binarioNaTela(indexFileName);
}

void indexCars()
{
    // Opening file names
    char binFileName[MAX_STRING_SIZE], indexFileName[MAX_STRING_SIZE];
    FILE *binFile = NULL, *indexFile = NULL;
    scanf("%s %s", binFileName, indexFileName);
    openFiles(&binFile, binFileName, "rb", &indexFile, indexFileName, "wb+", checkCarFileIntegrity);

    // Getting number of registers to be inserted
    int nreg = getCarNRegisters(binFile);

    // Creating containers
    Index *index = createIndex(indexFile);
    Car *c = newCar();
    Register *reg = newRegister();

    // Getting each car, then inserting its hash and offset into the index
    int offset = 0;
    for (int rrn = 0; rrn < nreg; rrn++)
    {
        offset = readCar(c, binFile, BIN, NO_OFFSET);
        if (!carLogicallyRemoved(c))
        {
            updateRegister(reg, getCarIndex(c), offset);
            insertRegister(index, reg);
        }
    }

    // Closing files
    freeCar(c);
    freeRegister(reg);
    closeIndex(index);
    fclose(binFile);

    binarioNaTela(indexFileName);
}