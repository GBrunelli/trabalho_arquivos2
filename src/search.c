#include "project.h"
#include "line.h"
#include "car.h"
#include "utils.h"
#include "b_tree.h"

void searchLine(void)
{
    // Scanning from stdin
    uint32_t indexedField;
    char binFileName[MAX_STRING_SIZE], indexFileName[MAX_STRING_SIZE];
    scanf("%s %s %*s %d", binFileName, indexFileName, &indexedField);

    // Opening files
    FILE *bin = NULL, *indexFile = NULL;
    openFiles(&bin, binFileName, "rb+", &indexFile, indexFileName, "rb+", checkLineFileIntegrity);

    Index* index = openIndex(indexFile);

    // Searching inside our index for the register
    Register *reg;
    Result result = searchRegister(index, indexedField, &reg);

    // If no result was found, finish program
    if (result == NOT_FOUND) {
        printf("Registro inexistente.\n");
    } else {
        // Otherwise print line using Pr obatined from the Index
        LineHeader *lh = newLineHeader();
        Line *l = newLine();

        updateLineHeader(lh, bin, BIN);
        updateLine(l, bin, BIN, getPR(reg)); 

        printLine(l, lh);

        // Freeing data that will not be used anymore
        freeLineHeader(lh);
        freeLine(l);
    }

    closeIndex(index);
    fclose(bin);
}

void searchCar(void)
{
    // Scanning from stdin
    char binFileName[MAX_STRING_SIZE], indexFileName[MAX_STRING_SIZE], indexedStringField[MAX_STRING_SIZE];
    scanf("%s %s %*s %s", binFileName, indexFileName, indexedStringField);

    // Transforming prefixo string into a hashed integer that can be used for searches in the index.
    removeQuotations(indexedStringField);
    int indexedField = convertePrefixo(indexedStringField);

    // Opening files
    FILE *bin = NULL, *indexFile = NULL;
    openFiles(&bin, binFileName, "rb+", &indexFile, indexFileName, "rb+", checkCarFileIntegrity);

    Index* index = openIndex(indexFile);

    // Searching inside our index for the register
    Register *reg;
    Result result = searchRegister(index, indexedField, &reg);

    // If no result was found, finish program
    if (result == NOT_FOUND) {
        printf("Registro inexistente.\n");
    } else {
        // Otherwise print line using Pr obatined from the Index
        CarHeader *ch = newCarHeader();
        Car *c = newCar();

        getCarHeader(ch, bin, BIN);
        readCar(c, bin, BIN, getPR(reg)); 

        printCar(c, ch);

        // Freeing data that will not be used anymore
        freeCarHeader(ch);
        freeCar(c);
    }

    closeIndex(index);
    fclose(bin);
}
