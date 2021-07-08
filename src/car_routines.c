#include "car.h"
#include "utils.h"

void generateIndexCars() {
    return;
}

void searchIndexedCar() {
    return;
}

void insertNewCarsThenIndex() {
    return;
}

// 5rd task -> SELECT WHERE Campo = Valor
void printSelectedCars()
{
    // Read the input and open the file
    char binFileName[MAX_STRING_SIZE], fieldName[MAX_STRING_SIZE], index[MAX_STRING_SIZE];
    scanf("%s %s", binFileName, fieldName);
    FILE *bin = fopen(binFileName, "rb");
    if (bin == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        exit(0);
    }

    // Checking file consistency
    CarHeader *header = newCarHeader();
    getCarHeader(header, bin, BIN);
    if (!checkCarFileIntegrity(header))
    {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        freeCarHeader(header);
        exit(0);
    }

    //If there are no registers, stop the processing and print "Registro inexistente."
    int nRegisters = getTotalNumberRegisters(bin);
    if (nRegisters == 0)
    {
        printf("Registro inexistente.\n");
        fclose(bin);
        freeCarHeader(header);
        exit(0);
    }


    // Print lines that match our search.
    Car *car = newCar();
    char match = 0;
    while (nRegisters--)
    {
        readCar(car, bin, BIN);
        // Checking whether line matches, then printing it if it isn't logically removed
        if (checkIfCarMatches(car, index))
        {
            printCar(car, header);
            printf("\n");
            match = 1;
        }
    }

    // Check whether matches were found
    if (!match)
        printf("Registro inexistente.\n");

    // Freeing data that will not be used anymore
    freeCarHeader(header);
    freeCar(car);
    fclose(bin);
}

// 7th task -> INSERT N REGISTERS INTO END
void InsertNewCarsBin()
{
    // Getting stdin data for number of new registers and filename
    int nRegisters = 0;
    char binFileName[MAX_STRING_SIZE];
    scanf("%s %d", binFileName, &nRegisters);

    // Opening both src and dest files
    FILE *bin = fopen(binFileName, "r+b");

    // Error handling if file didn't open correctly
    if (bin == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        exit(0);
    }

    // Checking file integrity
    CarHeader *header = newCarHeader();
    getCarHeader(header, bin, BIN);
    if (!checkCarFileIntegrity(header))
    {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        freeCarHeader(header);
        exit(0);
    }
    freeCarHeader(header);

    // Setting file as INCONSISTENT
    setFileStatus(bin, STATUS_INCONSISTENT);

    // Instantiating a Line and then writing all CSV Lines into BIN file
    Car *car = newCar();
    while (nRegisters--)
    {
        readCar(car, NULL, CLI);
        writeCar(car, bin, BIN);
    }
    freeCar(car);

    // Finishing program. Freeing all memory
    setFileStatus(bin, STATUS_CONSISTENT);
    fclose(bin);

    // Proof that our program worked correctly
    binarioNaTela(binFileName);
}