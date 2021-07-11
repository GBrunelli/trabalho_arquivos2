#include "project.h"
#include "line.h"
#include "car.h"
#include "utils.h"
#include "b_tree.h"

void indexLines() {
    // Opening file names
    char binFileName[MAX_STRING_SIZE], indexFileName[MAX_STRING_SIZE];
    FILE *binFile = NULL, *indexFile = NULL;
    scanf("%s %s", binFileName, indexFileName);
    openFiles(binFile, binFileName, "rb", indexFile, indexFileName, "wb+", checkLineFileIntegrity);

    // Getting number of registers to be inserted
    int nreg = getLineTotalRegisters(binFile);

    Index* index = createIndex(indexFile);
    Line *l = newLine();
    Register *reg = newRegister();
    for (int rrn = 0; rrn < nreg; rrn++) {
        updateLine(l, binFile, BIN);
        if (!logicallyRemoved(l)) {
            updateRegister(reg, getLineIndex(l), rrn);
            insertRegister(index, reg);
        }
    }
    closeIndex(index);
    freeLine(l);
    freeRegister(reg);
    fclose(binFile);

    binarioNaTela(indexFileName);
}

void indexCars() {
    // Opening file names
    char binFileName[MAX_STRING_SIZE], indexFileName[MAX_STRING_SIZE];
    FILE *binFile = NULL, *indexFile = NULL;
    scanf("%s %s", binFileName, indexFileName);
    openFiles(binFile, binFileName, "rb", indexFile, indexFileName, "wb+", checkLineFileIntegrity);

    // Getting number of registers to be inserted
    int nreg = getLineTotalRegisters(binFile);

    Index* index = createIndex(indexFile);
    Line *l = newLine();
    for (int rrn = 0; rrn < nreg; rrn++) {
        updateLine(l, binFile, BIN);
        if (!logicallyRemoved(l)) {
            Register *reg = createRegister(getLineIndex(l), rrn);
            insertRegister(index, reg);
            free(reg);
        }
    }

    freeLine(l);
    fclose(binFile);
    closeIndex(index);

    binarioNaTela(indexFileName);
}