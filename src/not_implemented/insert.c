#include "project.h"
#include "line.h"
#include "car.h"
#include "utils.h"
#include "b_tree.h"


void insertLines()
{
    // // Getting stdin data for number of new registers and filename
    // int nRegisters = 0;
    // char binFileName[MAX_STRING_SIZE], indexFileName[MAX_STRING_SIZE];
    // scanf("%s %s %d", binFileName, indexFileName, &nRegisters);

    // FILE *bin = NULL, *index = NULL;
    // openFiles(bin, binFileName, "r+b", index, indexFileName, "r+b", checkLineFileIntegrity);

    // // Setting file as INCONSISTENT
    // setLineFileStatus(bin, STATUS_INCONSISTENT);

    // // Writing all CSV Lines into BIN file
    // Line *l = newLine();
    // fseek(bin, 0, SEEK_END);
    // while (nRegisters--)
    // {
    //     // Getting register from CLI
    //     updateLine(l, NULL, CLI);
    //     int indexedField = getLineIndex(l);

    //     // Checking if indexedField is not already in use
    //     /* if (searchRegister(indexedField, NULL) != FOUND) */{
    //         // Inserting index into B Tree
    //         if (!logicallyRemoved(l))
    //             // insertRegister(indexedField, NULL);

    //         // Writing data to file
    //         writeLine(l, bin, BIN);
    //     }
    // }
    // freeLine(l);
    // fclose(index);

    // // Finishing program. Freeing all memory
    // setLineFileStatus(bin, STATUS_CONSISTENT);
    // fclose(bin);

    // // Proof that our program worked correctly
    // binarioNaTela(indexFileName);
}

// 7th task -> INSERT N REGISTERS INTO END
void insertCars()
{

}