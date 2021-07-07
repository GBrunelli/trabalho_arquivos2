#include "b_tree.h"

struct _IndexFile{
    FILE* indexFile;
    BTreeHeader header;
    BTreeNode *nodes;
};

struct _BTreeHeader
{
    char status;        // 1 byte
    int32_t noRaiz;     // 4 bytes
    int32_t RRNproxNo;  // 4 bytes
    // char[68]         // 68 bytes
};                      // 77 bytes total 


struct _BTreeNode
{
    // metadata
    char folha;                 // 1 byte
    int32_t nroChavesIndexadas; // 4 bytes
    int32_t RRNdoNo;            // 4 bytes

    // data
    int32_t P[ORDER];           // 20 bytes
    int32_t C[ORDER-1];         // 16 bytes
    int64_t Pr[ORDER-1];        // 32 bytes
};                              // 77 bytes



void _splitNode()
{

}

Result _insert(const int32_t currentRNN, 
               const int32_t key, 
               int32_t *promoKey, 
               int32_t *promoKeyChild)
{ 

}

BTreeNode *_getNode(const int32_t rnn)
{

}

/**
 * Makes a binary search in the node for the key, 
 * if found, returns the pos of the key,
 * else returns a negative number
*/
Result _binaryNodeSearch(const int32_t C[], const int32_t key, int32_t *P)
{

}

/**
 * Return the 
*/

int64_t _searchRegister(IndexFile *file,
                        const int32_t currentRNN, 
                        const int32_t key, 
                        int64_t *Pr,
                        int32_t *foundRNN,
                        int32_t *foundPos)
{
    // se current rnn nao é -1
        // retorna nao encontrado

    // senao 
        // leia o NODE apondatado por rnn na variavel bTreeNode
        // busca binaria por key no bTreeNode

        // se encontrar key
            // define FoundPos igual a posicao da da key (key == BTreeNode->C[FoundPos])
            // define FoundRnn igual a currentRNN
            // define *Pr = BTreeNode->Pr[FoundPos]
            // retorna Result.Found

        // senao
            // chamada recursiva

    if (currentRNN == -1)
        return NOT_FOUND;

    else 
    {
        int32_t pos;
        BTreeNode *node = _getNode(currentRNN);
        Result result = _binaryNodeSearch(node->C, key, &pos);

        if (result == FOUND)
        {
            *foundPos = pos;
            *foundRNN = currentRNN;
            *Pr = node->Pr[pos];
            return FOUND;
        }
        else
        {
            return _searchRegister(file, node->P[pos], key, Pr, foundRNN, foundPos);
        }
    }
}
