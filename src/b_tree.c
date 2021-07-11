#include "b_tree.h"

#define DISK_PAGE_SIZE 77

typedef struct _BTreeHeader IndexHeader;
typedef struct _BTreeNode DiskPage;

struct _Index
{
    FILE *indexFile;
    IndexHeader *header;
    DiskPage **savedPages;
    int nSavedPages;
};

struct _BTreeHeader
{
    bool status;
    int32_t noRaiz;
    int32_t RRNproxNo;
};

struct _BTreeNode
{
    // metadata
    bool folha;
    int32_t nroChavesIndexadas;
    int32_t RRNdoNo;

    // data
    int32_t P[ORDER];
    Register *regs[REGISTERS_PER_PAGE];
};

struct _Register
{
    int32_t C;
    int64_t Pr;
};
Result _insert(Index *index,
               int32_t currentRNN,
               Register *newReg,
               Register **promoReg,
               DiskPage **promoDiskPageChild);

DiskPage *_getPage(Index *index, int32_t rnn);

Result _binaryNodeSearch(DiskPage *page,
                         int32_t key,
                         Register **foundReg,
                         int *pos);

/**
 * Write "bytes * @" in the current file
*/
void _writeGarbage(FILE *file, int bytes)
{
    char garbage[1] = "@";
    for (int i = 0; i < bytes; i++)
        fwrite(&garbage, sizeof(char), 1, file);
}

/**
 * Write the header of the index in the file
*/
void _writeIndexHeader(Index *index)
{
    FILE *indexFile = index->indexFile;
    fseek(indexFile, 0, SEEK_SET);
    char status = index->header->status ? '1' : '0';
    fwrite(&status, 1, 1, indexFile);
    fwrite(&index->header->noRaiz, 4, 1, indexFile);
    fwrite(&index->header->RRNproxNo, 4, 1, indexFile);
    _writeGarbage(indexFile, 68);
}

/**
 * Write a disk page in the index file
*/
void _writeDiskPage(Index *index, DiskPage *page)
{
    int64_t offset = (page->RRNdoNo + 1) * DISK_PAGE_SIZE;
    FILE *indexFile = index->indexFile;
    fseek(indexFile, offset, SEEK_SET);
    char folha = page->folha ? '1' : '0';
    fwrite(&folha, sizeof(folha), 1, indexFile);
    fwrite(&page->nroChavesIndexadas, 4, 1, indexFile);
    fwrite(&page->RRNdoNo, 4, 1, indexFile);
    int32_t notFilled32 = -1;
    int64_t notFilled64 = -1;

    for (int i = 0; i < REGISTERS_PER_PAGE; i++)
    {
        fwrite(&page->P[i], 4, 1, indexFile);
        if (page->regs[i])
        {
            fwrite(&page->regs[i]->C, 4, 1, indexFile);
            fwrite(&page->regs[i]->Pr, 8, 1, indexFile);
        }
        else
        {
            fwrite(&notFilled32, 4, 1, indexFile);
            fwrite(&notFilled64, 8, 1, indexFile);
        }
    }
    fwrite(&page->P[4], 4, 1, indexFile);
}

/**
 * Opens an existing index file
*/
Index *openIndex(FILE *indexFile)
{
    Index *index = malloc(sizeof(Index));
    index->indexFile = indexFile;
    if (!index->indexFile)
    {
        free(index);
        return NULL;
    }
    index->header = malloc(sizeof(IndexHeader));
    index->savedPages = NULL;
    index->nSavedPages = 0;
    fseek(index->indexFile, 0, SEEK_SET);
    char status;
    fread(&status, 1, 1, index->indexFile);
    if (status == '0')
        return NULL;

    index->header->status = false;
    fread(&index->header->noRaiz, 4, 1, index->indexFile);
    fread(&index->header->RRNproxNo, 4, 1, index->indexFile);
    return index;
}

/**
 * Creates a new index file
*/
Index *createIndex(FILE *idx)
{
    Index *index = malloc(sizeof(Index));
    index->indexFile = idx;
    index->header = malloc(sizeof(IndexHeader));
    index->header->noRaiz = -1;
    index->header->RRNproxNo = 0;
    index->header->status = false;
    index->savedPages = NULL;
    index->nSavedPages = 0;
    _writeIndexHeader(index);
    return index;
}

/**
 * Closes the index file
*/
void closeIndex(Index *index)
{
    index->header->status = true;
    _writeIndexHeader(index);
    fclose(index->indexFile);
    free(index->header);
    for (int i = 0; i < index->nSavedPages; i++)
    {
        for (int j = 0; j < index->savedPages[i]->nroChavesIndexadas; j++)
        {
            free(index->savedPages[i]->regs[j]);
        }
        free(index->savedPages[i]);
    }
    free(index->savedPages);
    free(index);
}

/**
 * Retuns a new register
*/
Register *newRegister()
{
    Register *reg = calloc(1, sizeof(Register));
    return reg;
}

/**
 * Creates and returns a filled register
*/
Register *createRegister(int32_t C, int64_t Pr)
{
    Register *reg = malloc(sizeof(Register));
    reg->C = C;
    reg->Pr = Pr;
    return reg;
}

/**
 * Get the offset of the register
*/
int64_t getPR(Register *reg)
{
    return reg->Pr;
}

/**
 * Updates the register with the given values
*/
void updateRegister(Register *reg, int32_t C, int64_t Pr)
{
    reg->C = C;
    reg->Pr = Pr;
}

/*
 * compate 2 **register
*/
int compareRegisters(const void *a, const void *b)
{
    Register **regA = (Register **)a;
    Register **regB = (Register **)b;
    return (*regA)->C - (*regB)->C;
}

/**
 * Free a register from memory
*/
void freeRegister(Register *r)
{
    free(r);
}

/**
 * Creates a new disk page for a given index file, with the correct rnn
*/
DiskPage *_createDiskPage(Index *index, bool folha)
{
    DiskPage *page = malloc(sizeof(DiskPage));
    page->folha = folha;
    page->nroChavesIndexadas = 1;
    for (int i = 0; i < ORDER; i++)
        page->P[i] = -1;
    for (int i = 0; i < REGISTERS_PER_PAGE; i++)
        page->regs[i] = NULL;

    page->RRNdoNo = index->header->RRNproxNo++;
    return page;
}

/**
 * Saves a disk page in memory for fast access 
*/
void _saveDiskPageInMemory(Index *index, DiskPage *page)
{
    index->savedPages = realloc(index->savedPages, (++index->nSavedPages) * sizeof(DiskPage *));
    index->savedPages[(index->nSavedPages) - 1] = page;
}

/**
 * Insert a register in the index file, keeping the btree balanced
*/
Result insertRegister(Index *index, Register *reg)
{
    Register *regcpy = malloc(sizeof(Register));
    memcpy(regcpy, reg, sizeof(Register));

    // if there is no root, creates a new btree
    if (index->header->noRaiz == -1)
    {
        index->header->noRaiz = 0;
        DiskPage *page = _createDiskPage(index, true);
        page->regs[0] = regcpy;
        _saveDiskPageInMemory(index, page);
        _writeDiskPage(index, page);
    }
    // else, insert the new register in the existing btree
    else
    {
        DiskPage *promoDiskPageChild;
        Result resultValue = _insert(index, index->header->noRaiz, regcpy, &regcpy, &promoDiskPageChild);
        if (resultValue == ERROR)
            free(regcpy);
        return resultValue;
    }
    return ERROR;
}

/**
 * Split a disk page in two
*/
void _splitNode(Index *index,
                Register *newReg,
                DiskPage *currentPage,
                Register **promoRegister,
                DiskPage **promoDiskPageChild)
{
    // keep the old pointer
    int32_t oldDiskPageChildRNN = (*promoDiskPageChild) ? (*promoDiskPageChild)->RRNdoNo : -1;

    // Creates a register array with all the registers
    Register *regs[REGISTERS_PER_PAGE + 1];
    memcpy(regs, currentPage->regs, sizeof(currentPage->regs));

    // Create a array with all pointers
    int32_t P[ORDER + 1];
    memcpy(P, currentPage->P, sizeof(currentPage->P));

    // finds the correct position of the the registers
    bool inserted = false;
    int pos = REGISTERS_PER_PAGE;
    for (int i = 0; (i < REGISTERS_PER_PAGE) && (!inserted); i++)
    {
        if (newReg->C < regs[i]->C)
        {
            pos = i;
            inserted = true;
        }
    }

    // make the correction of the registers and pointers
    if (inserted)
        for (int i = currentPage->nroChavesIndexadas; i > pos; i--)
        {
            regs[i] = regs[i - 1];
            P[i + 1] = P[i];
        }

    // insert the new register in the array
    regs[pos] = newReg;
    P[pos + 1] = oldDiskPageChildRNN;

    // set the promo register as the middle register
    (*promoRegister) = regs[REGISTERS_PER_PAGE / 2];

    // remake the current page
    currentPage->regs[0] = regs[0];
    currentPage->regs[1] = regs[1];
    currentPage->regs[2] = NULL;
    currentPage->regs[3] = NULL;
    currentPage->P[0] = P[0];
    currentPage->P[1] = P[1];
    currentPage->P[2] = P[2];
    currentPage->P[3] = -1;
    currentPage->P[4] = -1;
    currentPage->nroChavesIndexadas = 2;

    // create the right page
    (*promoDiskPageChild) = _createDiskPage(index, currentPage->folha);
    (*promoDiskPageChild)->regs[0] = regs[3];
    (*promoDiskPageChild)->regs[1] = regs[4];
    (*promoDiskPageChild)->regs[2] = NULL;
    (*promoDiskPageChild)->regs[3] = NULL;
    (*promoDiskPageChild)->P[0] = P[3];
    (*promoDiskPageChild)->P[1] = P[4];
    (*promoDiskPageChild)->P[2] = P[5];
    (*promoDiskPageChild)->nroChavesIndexadas = 2;
    _saveDiskPageInMemory(index, (*promoDiskPageChild));
}

/**
 * Recursive function to insert a value while keeping it balanced
*/
Result _insert(Index *index,
               int32_t currentRNN,
               Register *newReg,
               Register **promoReg,
               DiskPage **promoDiskPageChild)
{
    
    if (currentRNN == -1)
    {
        (*promoReg) = newReg;
        (*promoDiskPageChild) = NULL;
        return PROMOTION;
    }
    else
    {   
        // stacking/search phase, keep calling the function until a leaf node is founded
        DiskPage *currentPage = _getPage(index, currentRNN);
        Register *foundReg;
        int pos;
        if (_binaryNodeSearch(currentPage, (*promoReg)->C, &foundReg, &pos) == FOUND)
            return ERROR;

        Result returnValue = _insert(index, currentPage->P[pos], (*promoReg), promoReg, promoDiskPageChild); 
        // if we are done with the spliting, we start to unstack everthing
        if ((returnValue == NO_PROMOTION) || (returnValue == ERROR))
        {
            return returnValue;
        }
        // if the nodes has space for 1 more register, insert it
        if (currentPage->nroChavesIndexadas < REGISTERS_PER_PAGE)
        {
            // insert the pointers and register in order
            for (int i = currentPage->nroChavesIndexadas; i > pos; i--)
            {
                currentPage->regs[i] = currentPage->regs[i - 1];
                currentPage->P[i + 1] = currentPage->P[i];
            }
            currentPage->regs[pos] = (*promoReg);
            currentPage->P[pos + 1] = (*promoDiskPageChild) != NULL ? (*promoDiskPageChild)->RRNdoNo : -1;
            currentPage->nroChavesIndexadas++;
            _writeDiskPage(index, currentPage);
            // indicates that no promotion was made, and we can start unstacking the recursion with no more splits
            return NO_PROMOTION;
        }
        // if the nodes is full, we split the current node, and promotes the mid value
        else
        {
            _splitNode(index, (*promoReg), currentPage, promoReg, promoDiskPageChild);
            // if the current node was the root, we need to create a new root
            if (currentPage->RRNdoNo == index->header->noRaiz)
            {
                DiskPage *newRoot = _createDiskPage(index, false);
                newRoot->regs[0] = (*promoReg);
                newRoot->P[0] = currentPage->RRNdoNo;
                newRoot->P[1] = (*promoDiskPageChild)->RRNdoNo;
                index->header->noRaiz = newRoot->RRNdoNo;
                _saveDiskPageInMemory(index, newRoot);
                _writeDiskPage(index, newRoot);
            }
            _writeDiskPage(index, currentPage);
            _writeDiskPage(index, *promoDiskPageChild);
            // indicates that an promotion was made, for the recursion call before this can try 
            // to insert the promoted register in the level above, and so on
            return PROMOTION;
        }
    }
}

/**
 * Gets a node from the index file given its rnn
*/
DiskPage *_getPage(Index *index, int32_t rnn)
{
    int nPages = index->nSavedPages;
    for (int i = 0; i < nPages; i++)
    {
        if (index->savedPages[i]->RRNdoNo == rnn)
            return index->savedPages[i];
    }

    // if the disk page is not on memory, recover it

    // allocates the new page
    DiskPage *page = malloc(sizeof(DiskPage));
    for (int i = 0; i < ORDER; i++)
        page->P[i] = -1;

    // read and saves the fields
    fseek(index->indexFile, (rnn + 1) * DISK_PAGE_SIZE, SEEK_SET);
    char folha;
    int32_t nroChavesIndexadas;
    int32_t RRNdoNo;
    fread(&folha, 1, 1, index->indexFile);
    fread(&nroChavesIndexadas, 4, 1, index->indexFile);
    fread(&RRNdoNo, 4, 1, index->indexFile);
    page->folha = folha == '1' ? true : false;
    page->nroChavesIndexadas = nroChavesIndexadas;
    page->RRNdoNo = RRNdoNo;
    for (int i = 0; i < REGISTERS_PER_PAGE; i++)
    {
        fread(&page->P[i], 4, 1, index->indexFile);
        int32_t C;
        int64_t Pr;
        fread(&C, 4, 1, index->indexFile);
        fread(&Pr, 8, 1, index->indexFile);
        bool CIsGarbage = (C == -1);
        bool PrIsGarbage = (Pr == -1);
        if (!(CIsGarbage || CIsGarbage))
            page->regs[i] = createRegister(CIsGarbage ? -1 : C,
                                           PrIsGarbage ? -1 : Pr);
        else
            break;
    }
    fread(&page->P[REGISTERS_PER_PAGE], 4, 1, index->indexFile);
    _saveDiskPageInMemory(index, page);
    return page;
}

/**
 * Makes a binary search in the node for the key, if found, 
 * returns FOUND, the pos of the key, and the found register
 * else returns NOT_FOUND
*/
Result _binaryNodeSearch(DiskPage *page,
                         int32_t key,
                         Register **foundReg,
                         int *pos)
{
    int begin = 0;
    int end = page->nroChavesIndexadas - 1;
    int middle = 0;
    while (begin <= end)
    {
        middle = (begin + end) / 2;
        if (page->regs[middle]->C == key)
        {
            *foundReg = page->regs[middle];
            return FOUND;
        }
        if (page->regs[middle]->C < key)
            begin = middle + 1;
        else
            end = middle - 1;
    }
    if (pos != NULL)
    {
        if (page->regs[middle]->C > key)
            *pos = middle;
        else
            *pos = middle + 1;
    }
    return NOT_FOUND;
}

/**
 * Recursive function for searching the btree
*/
Result _searchRegister(Index *index,
                       int32_t currentRNN,
                       int32_t key,
                       Register **foundReg)
{
    // the we reach a leaf, there is no register with the given key
    if (currentRNN == -1)
        return NOT_FOUND;

    else
    {
        // keep calling the functino for the child node
        int pos;
        DiskPage *node = _getPage(index, currentRNN);
        Result result = _binaryNodeSearch(node, key, foundReg, &pos);
        if (result == FOUND)
            return FOUND;
        else
            return _searchRegister(index, node->P[pos], key, foundReg);
    }
}

/**
 * Searches the btree for a key, if found, returns FOUND, 
 * and the found register, else returns NOT_FOUND
*/
Result searchRegister(Index *index, int32_t key, Register **foundReg)
{
    return _searchRegister(index, index->header->noRaiz, key, foundReg);
}
