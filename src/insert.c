#include "project.h"
#include "line.h"
#include "car.h"
#include "utils.h"
#include "b_tree.h"


void insertLines()
{
    // Getting stdin data for number of new registers and filename
    int nRegisters = 0;
    char binFileName[MAX_STRING_SIZE], indexFileName[MAX_STRING_SIZE];
    scanf("%s %s %d", binFileName, indexFileName, &nRegisters);

    FILE *binFile = NULL, *indexFile = NULL;
    openFiles(&binFile, binFileName, "r+b", &indexFile, indexFileName, "r+b", checkLineFileIntegrity);

    // Setting file as INCONSISTENT
    setLineFileStatus(binFile, STATUS_INCONSISTENT);

    // Writing all CSV Lines into BIN file
    Line *l = newLine();
    Register *reg = newRegister();
    Index *index = openIndex(indexFile);

    fseek(binFile, 0, SEEK_END);
    int offset = 0;
    while (nRegisters--)
    {
        // Getting register from CLI
        updateLine(l, NULL, CLI, NO_OFFSET);
        int indexedField = getLineIndex(l);

        // Writing data to file
        offset = writeLine(l, binFile, BIN);

        // Inserting index into B Tree
        if (!lineLogicallyRemoved(l)) {
            updateRegister(reg, indexedField, offset);
            insertRegister(index, reg);
        }
    }
    freeLine(l);
    freeRegister(reg);
    closeIndex(index);

    // Finishing program. Freeing all memory
    setLineFileStatus(binFile, STATUS_CONSISTENT);
    fclose(binFile);

    // Proof that our program worked correctly
    binarioNaTela(indexFileName);
}

// 7th task -> INSERT N REGISTERS INTO END
void insertCars()
{
    // Getting stdin data for number of new registers and filename
    int nRegisters = 0;
    char binFileName[MAX_STRING_SIZE], indexFileName[MAX_STRING_SIZE];
    scanf("%s %s %d", binFileName, indexFileName, &nRegisters);

    FILE *binFile = NULL, *indexFile = NULL;
    openFiles(&binFile, binFileName, "r+b", &indexFile, indexFileName, "r+b", checkCarFileIntegrity);

    // Setting file as INCONSISTENT
    setCarFileStatus(binFile, STATUS_INCONSISTENT);

    // Writing all CSV Cars into BIN file
    Car *c = newCar();
    Register *reg = newRegister();
    Index *index = openIndex(indexFile);

    fseek(binFile, 0, SEEK_END);

    int offset = 0;
    while (nRegisters--)
    {
        // Getting register from CLI
        readCar(c, NULL, CLI, NO_OFFSET);
        int indexedField = getCarIndex(c);

        // Writing data to file
        offset = writeCar(c, binFile, BIN);

        // Inserting index into B Tree
        if (!carLogicallyRemoved(c)) {
            updateRegister(reg, indexedField, offset);
            insertRegister(index, reg);
        }
    }
    freeCar(c);
    freeRegister(reg);
    closeIndex(index);

    // Finishing program. Freeing all memory
    setCarFileStatus(binFile, STATUS_CONSISTENT);
    fclose(binFile);

    // Proof that our program worked correctly
    binarioNaTela(indexFileName);
}