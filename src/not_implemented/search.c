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
    FILE *bin = NULL, *index = NULL;
    openFiles(bin, binFileName, "rb", index, indexFileName, "rb", checkLineFileIntegrity);

    // Searching inside our index for the register 
    Result result = 0; /* searchRegister(indexedField, NULL); */
    fclose(index);

    // If no result was found, finish program
    if (result == NOT_FOUND) {
        printf("Registro inexistente.\n");
        fclose(bin);
        exit(0);
    }

    // Otherwise print register
    // ! Precisa adicionar maneira de achar por índice exato
    LineHeader *lh = newLineHeader();
    Line *l = newLine();
    updateLineHeader(lh, bin, BIN);
    updateLine(l, bin, BIN/*, RRN*/ ); 

    printLine(l, lh);

    // Freeing data that will not be used anymore
    freeLineHeader(lh);
    freeLine(l);
    fclose(bin);
}

void searchCar(void)
{
    // // Read the input and open the file
    // char binFileName[MAX_STRING_SIZE], fieldName[MAX_STRING_SIZE], index[MAX_STRING_SIZE];
    // scanf("%s %s", binFileName, fieldName);
    // FILE *bin = fopen(binFileName, "rb");
    // if (bin == NULL)
    // {
    //     printf("Falha no processamento do arquivo.\n");
    //     exit(0);
    // }

    // // Checking file consistency
    // CarHeader *header = newCarHeader();
    // getCarHeader(header, bin, BIN);
    // if (!checkCarFileIntegrity(header))
    // {
    //     printf("Falha no processamento do arquivo.\n");
    //     fclose(bin);
    //     freeCarHeader(header);
    //     exit(0);
    // }

    // //If there are no registers, stop the processing and print "Registro inexistente."
    // int nRegisters = getTotalNumberRegisters(bin);
    // if (nRegisters == 0)
    // {
    //     printf("Registro inexistente.\n");
    //     fclose(bin);
    //     freeCarHeader(header);
    //     exit(0);
    // }


    // // Print lines that match our search.
    // Car *car = newCar();
    // char match = 0;
    // while (nRegisters--)
    // {
    //     readCar(car, bin, BIN);
    //     // Checking whether line matches, then printing it if it isn't logically removed
    //     if (checkIfCarMatches(car, index))
    //     {
    //         printCar(car, header);
    //         printf("\n");
    //         match = 1;
    //     }
    // }

    // // Check whether matches were found
    // if (!match)
    //     printf("Registro inexistente.\n");

    // // Freeing data that will not be used anymore
    // freeCarHeader(header);
    // freeCar(car);
    // fclose(bin);
}
