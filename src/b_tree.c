#include "b_tree.h"

struct _BTreeHeader
{
    char status;        // 1 byte
    int32_t noRaiz;     // 4 bytes
    int32_t RRNproxNo;  // 4 bytes
    // char[68]         // 68 bytes
};                      // 77 bytes total 


struct _BTreeRegister
{
    // metadata
    char folha;                 // 1 byte
    int32_t nroChavesIndexadas; // 4 bytes
    int32_t RRNdoNo;            // 4 bytes

    // data
    int32_t P[5];               // 20 bytes
    int32_t C[4];               // 16 bytes
    int64_t Pr[4]               // 32 bytes
};                              // 77 bytes

