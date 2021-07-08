#ifndef LINE_H
#define LINE_H

#include "project.h"
#include "utils.h"

typedef struct _LineHeader LineHeader;

typedef struct _Line Line;

/* ## Functions to deal with line headers ## */

// Create a new line header
LineHeader *newLineHeader();

// Get all Header information from a specific source file.
// Currently supported sources: BIN, CSV
void updateLineHeader(LineHeader *lh, FILE *file, Source from);

// Check whether LineHeader status is '1' or '0'
bool checkFileIntegrity(LineHeader *lh);

// Overwrite old LineHeader from file with a newer, currently in-memory one.
void overwriteLineHeader(LineHeader *lh, FILE *file, Source source);

// Returns how many non logically removed registers there are.
int getNRegisters(LineHeader *lh);

// Returns how many logically removed registers there are
int getNRemovedRegisters(LineHeader *lh);

// Free all data associated with a line register
void freeLineHeader(LineHeader *lh);

// Set a binary line file as INCONSISTENT or CONSISTENT, depending on supplied C
void setLineFileStatus(FILE *file, char c);

/* ## Basic line functions ## */

// Creates a new reusable Line.
Line *newLine();

// Free all memory associated with a line
void freeLine(Line *l);

// Prints Line. Also checks if Line is logically removed and deals with nulls.
FuncStatus printLine(Line *l, LineHeader *lh);

// Updates a Line with data from a specific source.
// Currently supported sources: BIN, CLI, CSV
// If updating from CLI, file should be NULL.
FuncStatus updateLine(Line *l, FILE *file, Source from);

// Writes a Line to a specific source
// Currently only supports BIN files.
FuncStatus writeLine(Line *l, FILE *file, Source from);

// Returns the index (codLinha)
int32_t getLineIndex(Line *l);

// Checks whether a line is logically removed
bool logicallyRemoved(Line *l);

/* ## Functions related to searching using a specific struct field ## */

// Based on an specific searched value, check whether the current line matches
bool checkIfLineMatches(Line *l, char* index);

#endif