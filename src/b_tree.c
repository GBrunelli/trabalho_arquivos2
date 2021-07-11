#include "b_tree.h"

#define DISK_PAGE_SIZE 77
#define INT_32_GARBAGE 1077952576
#define INT_64_GARBAGE 4629771061636907072

typedef struct _BTreeHeader IndexHeader;
typedef struct _BTreeNode DiskPage;

struct _Index
{
    FILE* indexFile;
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
    Register **regs;
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

void _writeGarbage(FILE* file, int bytes)
{
    char garbage[1] = "@";
    for (int i = 0; i < bytes; i++) fwrite(&garbage, sizeof(char), 1, file);  
}

void _writeIndexHeader(Index* index)
{
    FILE* indexFile = index->indexFile;
    fseek(indexFile, 0, SEEK_SET);
    char status = index->header->status ? '1' : '0';
    fwrite(&status, 1, 1, indexFile);
    fwrite(&index->header->noRaiz, 4, 1, indexFile);
    fwrite(&index->header->RRNproxNo, 4, 1, indexFile);
    _writeGarbage(indexFile, 68);
}

void _writeDisckPage(Index* index, DiskPage* page)
{
    int64_t offset = page->RRNdoNo * DISK_PAGE_SIZE;
    FILE* indexFile = index->indexFile;
    fseek(indexFile, offset, SEEK_SET);
    char folha = page->folha ? '1' : '0';
    fwrite(&folha, sizeof(folha), 1, indexFile);
    fwrite(&page->nroChavesIndexadas, 4, 1, indexFile);
    fwrite(&page->RRNdoNo, 4, 1, indexFile);

    for (int i = 0; i < REGISTERS_PER_PAGE; i++)
    {
        fwrite(&page->P[i], 4, 1, indexFile);
        if(page->regs[i])
        {
            fwrite(&page->regs[i]->C,  4, 1, indexFile);
            fwrite(&page->regs[i]->Pr, 8, 1, indexFile);
        }
        else
            _writeGarbage(indexFile, 12);
    }
    fwrite(&page->P[4], 4, 1, indexFile);
}

Index *openIndex(char *indexFileName)
{
    Index *index = malloc(sizeof(Index));
    index->indexFile = fopen(indexFileName, "rb+");
    if(!index->indexFile)
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
    if(status == '0') return NULL;

    index->header->status = false;
    fread(&index->header->noRaiz,    4, 1, index->indexFile);
    fread(&index->header->RRNproxNo, 4, 1, index->indexFile);
    return index;
}

Index *createIndex(FILE* idx)
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

void closeIndex(Index *index)
{
    index->header->status = true;
    _writeIndexHeader(index);
    fclose(index->indexFile);
    free(index->header);
    for (int i = 0; i < index->nSavedPages; i++)
    {
        for (int j = 0; j < REGISTERS_PER_PAGE; j++)
            free(index->savedPages[i]->regs[j]);
        free(index->savedPages[i]->regs);
        free(index->savedPages[i]);
    }
    free(index->savedPages);
    free(index);
}

Register* newRegister() {
    Register *reg = calloc(1, sizeof(Register));
    return reg;
}

Register *createRegister(int32_t C, int64_t Pr)
{
    Register *reg = malloc(sizeof(Register));
    reg->C = C;
    reg->Pr = Pr;
    return reg;
}

void updateRegister(Register* reg, int32_t C, int64_t Pr) {
    reg->C = C;
    reg->Pr = Pr;
}

int compareRegisters(const void * a, const void * b)
{
    Register **regA = (Register**) a;
    Register **regB = (Register**) b;
    return (*regA)->C - (*regB)->C;
}

void freeRegister(Register *r) {
    free(r);
}

DiskPage *_createDiskPage(Index *index, bool folha)
{
    DiskPage *page = malloc(sizeof(DiskPage));
    page->regs = calloc(1, sizeof(Register*)*REGISTERS_PER_PAGE);
    page->folha = folha;
    page->nroChavesIndexadas = 1;
    for (int i = 0; i < ORDER; i++)
        page->P[i] = -1;
    
    page->RRNdoNo = ++index->header->RRNproxNo;
    return page;
}

void _saveDiskPageInMemory(Index *index, DiskPage *page)
{
    index->savedPages = realloc(index->savedPages, (++index->nSavedPages) * sizeof(DiskPage*));
    index->savedPages[(index->nSavedPages)-1] = page;
}

Result insertRegister(Index *index, Register *reg)
{
    if(index->header->noRaiz == -1)
    {
        index->header->noRaiz = 1; 
        DiskPage *page = _createDiskPage(index, true);
        page->regs[0] = reg;
        _saveDiskPageInMemory(index, page);
        _writeDisckPage(index, page);
    }
    else 
    {
        DiskPage *promoDiskPageChild;
        _insert(index, index->header->noRaiz, reg, &reg, &promoDiskPageChild);
    }

    // ? INSERI ISSO AQUI PQ NÃO COMPILAVA SEM
    return FOUND;
}

void _splitNode(Index *index,
                Register *newReg,
                DiskPage *currentDiskPage,
                Register **promoRegister,
                DiskPage **rightDiskPage)
{
    // Creates a register array with all the registers and sort it 
    Register **regs = malloc(sizeof(Register*)*ORDER);

    for (int i = 0; i < REGISTERS_PER_PAGE; i++)
        regs[i] = currentDiskPage->regs[i];
    regs[REGISTERS_PER_PAGE] = newReg;

    qsort(regs, ORDER, sizeof(Register*), compareRegisters);

    // find the middle register
    (*promoRegister) = regs[REGISTERS_PER_PAGE/2];

    // remake the current page
    currentDiskPage->regs[2] = NULL;
    currentDiskPage->regs[3] = NULL;
    currentDiskPage->nroChavesIndexadas = 2;

    // keep the old pointer
    int32_t oldDiskPageChildRNN = (*rightDiskPage) ? (*rightDiskPage)->RRNdoNo : -1;

    // fill the right page
    (*rightDiskPage) = _createDiskPage(index, currentDiskPage->folha);
    (*rightDiskPage)->regs[0] = regs[3];
    (*rightDiskPage)->regs[1] = regs[4];
    (*rightDiskPage)->P[0] = currentDiskPage->P[3];
    (*rightDiskPage)->P[1] = currentDiskPage->P[4];
    (*rightDiskPage)->P[2] = oldDiskPageChildRNN;
    (*rightDiskPage)->nroChavesIndexadas = 2;
    _saveDiskPageInMemory(index, (*rightDiskPage));

    currentDiskPage->P[3] = -1;
    currentDiskPage->P[4] = -1;

    free(regs);
}

Result _insert(Index *index,
               int32_t currentRNN, 
               Register *newReg,
               Register **promoReg, 
               DiskPage **promoDiskPageChild)
{
    if(currentRNN == -1)
    {
        (*promoReg) = newReg;
        (*promoDiskPageChild) = NULL;
        return PROMOTION;
    }
    else
    {
        DiskPage *currentPage = _getPage(index, currentRNN);
        Register *foundReg;
        int pos;
        if(_binaryNodeSearch(currentPage, (*promoReg)->C, &foundReg, &pos) == FOUND)
            return ERROR;

        Result returnValue = _insert(index, currentPage->P[pos], (*promoReg), promoReg, promoDiskPageChild);
        if((returnValue == NO_PROMOTION) || (returnValue == ERROR)) 
        { 
            return returnValue; 
        }
        if(currentPage->nroChavesIndexadas < REGISTERS_PER_PAGE)
        {
            // caso nao for folha, faz a correção dos ponteiros se necessário
            if(!currentPage->folha)
            {
                int n = currentPage->nroChavesIndexadas;
                for (int i = n; i > pos; i++)
                {
                    currentPage->regs[i] = currentPage->regs[i-1];
                    currentPage->P[i+1] - currentPage->P[i];
                }
            }
            currentPage->regs[pos] = (*promoReg);
            currentPage->P[pos+1] = (*promoDiskPageChild) != NULL ? (*promoDiskPageChild)->RRNdoNo : -1;
            currentPage->nroChavesIndexadas++;
            _writeDisckPage(index, currentPage);
            return NO_PROMOTION;
        }
        else
        {
            _splitNode(index, (*promoReg), currentPage, promoReg, promoDiskPageChild);
            if(currentPage->RRNdoNo == index->header->noRaiz)
            {
                DiskPage *newRoot = _createDiskPage(index, false);
                newRoot->regs[0] = (*promoReg);
                newRoot->P[0] = currentPage->RRNdoNo;
                newRoot->P[1] = (*promoDiskPageChild)->RRNdoNo;
                index->header->noRaiz = newRoot->RRNdoNo;
                _saveDiskPageInMemory(index, newRoot);
                _writeDisckPage(index, newRoot);
            }
            _writeDisckPage(index, currentPage);
            _writeDisckPage(index, *promoDiskPageChild);
            return PROMOTION;
        }
    }
}

DiskPage *_getPage(Index *index, int32_t rnn)
{
    int nPages = index->nSavedPages;
    while (nPages)
    {
        if (index->savedPages[nPages-1]->RRNdoNo == rnn) 
        {
            return index->savedPages[nPages-1];
        }
        nPages--;
    }
    // if the disk page is not on memory, recover it
    DiskPage *page = malloc(sizeof(DiskPage));
    page->regs = calloc(1, sizeof(Register*)*REGISTERS_PER_PAGE);
    for (int i = 0; i < ORDER; i++)
        page->P[i] = -1;

    fseek(index->indexFile, rnn*DISK_PAGE_SIZE, SEEK_SET);

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
        bool CIsGarbage = (C == INT_32_GARBAGE);
        bool PrIsGarbage = (Pr == INT_64_GARBAGE);
        if(!(CIsGarbage || CIsGarbage))
            page->regs[i] = createRegister(CIsGarbage ? INT_32_GARBAGE : C, 
            PrIsGarbage ? INT_64_GARBAGE : Pr);
        else break;
    }
    fread(&page->P[REGISTERS_PER_PAGE], 4, 1, index->indexFile);
    _saveDiskPageInMemory(index, page);
    return page;
}

/**
 * Makes a binary search in the node for the key, 
 * if found, returns the pos of the key,
 * else returns NOT_FOUND
*/
Result _binaryNodeSearch(DiskPage *page,
                         int32_t key, 
                         Register **foundReg, 
                         int *pos)
{
    int begin = 0;
    int end = page->nroChavesIndexadas-1;
    int middle = 0;
    while(begin <= end)
    {
        middle = (begin + end) / 2;
        if(page->regs[middle]->C == key)
        {
            *foundReg = page->regs[middle];
            return FOUND;
        }
        if(page->regs[middle]->C < key)
            begin = middle + 1;
        else
            end = middle - 1;
    }
    if(pos != NULL)
    {
        if(page->regs[middle]->C > key) 
            *pos = middle;
        else 
            *pos = middle+1;
    }   
    return NOT_FOUND;
}

Result _searchRegister(Index *index,
                       int32_t currentRNN,
                       int32_t key,
                       Register **foundReg)
{
    if (currentRNN == -1)
        return NOT_FOUND;

    else 
    {
        int pos;
        DiskPage *node = _getPage(index, currentRNN);
        Result result = _binaryNodeSearch(node, key, foundReg, &pos);

        if (result == FOUND)
        {
            return FOUND;
        }
        else
            return _searchRegister(index, node->P[pos], key, foundReg);
    }
}

Result searchRegister(Index *index, int32_t key, Register **foundReg)
{
    return _searchRegister(index, index->header->noRaiz, key, foundReg);
}
