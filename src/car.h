#ifndef CAR_H
#define CAR_H

#include "project.h"
#include "utils.h"

typedef struct _CarHeader CarHeader;

typedef struct _Car Car;

/* ## Functions to deal with Car headers ## */

// Gets the sum of active and removed register in the bin file
int getCarNRegisters(FILE *file);

// Alocates memory and initializes the struct CarHeader
CarHeader *newCarHeader();

// Get all Header information from a specific source file.
// Currently supported sources: BIN, CSV
CarHeader *getCarHeader(CarHeader *carHeader, FILE *file, Source from);

// Set the status of a file as consistent '1' or inconsistent '0'
void setCarFileStatus(FILE *file, char c);

// Verify if the file is consistent.
// Returns 0 if it is inconsistent, or a value
// different than zero if it is consistent
int checkCarHeaderIntegrity(CarHeader *ch);

// Check whether a binary car file is corrupted.
bool checkCarFileIntegrity(FILE *bin);

/* ## Basic Car functions ## */

// Alocates memory and initializes the struct Car
Car *newCar();

// Reads a car at the current file pointer from a source file. For bin files, if
// the pointer is pointing at the header, it will read the first car in the file.
int64_t readCar(Car *c, FILE *file, Source From, int64_t pre_offset);

// Prints Car. Checks if Car is logically removed and also deals with nulls.
int printCar(Car *car, CarHeader *header);

// Checks whether a car is logically removed
bool carLogicallyRemoved(Car *c);

// Returns the index (prefixo) transformed into an integer
int32_t getCarIndex(Car *c);

// Free all memory associated with a Car.
void freeCar(Car *c);

// Free all memory associated with a CarHeader.
void freeCarHeader(CarHeader *carHeader);

/* ## Functions related to writing Cars to different sources ## */

// Writes a Car to a specific source
// Currently only supports BIN files.
int64_t writeCar(Car *car, FILE *file, Source from);

// Writes a CarHeader to a specific source
// Currently only supports BIN files.
void writeCarHeader(CarHeader *carHeader, FILE *file, Source from);

#endif