#ifndef B_TREE_H
#define B_TREE_H

#include "project.h"

#define ORDER 5

typedef enum _Result
{
    PROMOTION = -100,
    NO_PROMOTION,
    FOUND,
    NOT_FOUND,
    ERROR
} Result;

typedef struct _BTreeHeader BTreeHeader;
typedef struct _BTreeNode BTreeNode;
typedef struct _IndexFile IndexFile;

BTreeNode newBTreeNode();

Result insertRegister(const int32_t key);

Result searchRegister(const int32_t key, int64_t *Pr);

#endif