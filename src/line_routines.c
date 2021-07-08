#include "line.h"
#include "utils.h"
#include "b_tree.h"

void _openLineFiles(FILE* bin, char* binFileName, char* binMode, FILE* index, char* indexFileName, char* indexMode) {
    bin = fopen(binFileName, binMode);
    if (bin == NULL)
    {
        printf("Falha no processamento do arquivo.\n");
        exit(0);
    }

    index = fopen(indexFileName, indexMode);
    if (index == NULL)
    {
        fclose(bin);
        printf("Falha no processamento do arquivo.\n");
        exit(0);
    }

    // Checking file integrity
    LineHeader *lh = newLineHeader();
    updateLineHeader(lh, bin, BIN);
    if (!checkFileIntegrity(lh))
    {
        printf("Falha no processamento do arquivo.\n");
        fclose(bin);
        fclose(index);
        freeLineHeader(lh);
        exit(0);
    }
    freeLineHeader(lh);
}

void generateIndexLines() {
    // Opening file names
    char binFileName[MAX_STRING_SIZE], indexFileName[MAX_STRING_SIZE];
    FILE *bin = NULL, *index = NULL;
    scanf("%s %s", binFileName, indexFileName);
    _openLineFiles(bin, binFileName, "rb", index, indexFileName, "w+b");

    // Getting number of registers to be inserted
    LineHeader *lh = newLineHeader();
    updateLineHeader(lh, bin, BIN);
    int nreg = getNRegisters(lh) + getNRemovedRegisters(lh);
    freeLineHeader(lh);

    // btree = newBTree();
    BTreeNode* node = newBTreeNode();
    Line *l = newLine();
    for (int rrn = 0; rrn < nreg; rrn++) {
        updateLine(l, bin, BIN);
        if (!logicallyRemoved(l)) {
            // LineToBTreeNode(node, line, rrn);
            // insertNodeIntoBTree(btree, node);
        }
    }
    freeLine(l);
    fclose(bin);

    // SaveBTreeIntoIndexFile(index, btree);

    freeBTreeNode(node);
    fclose(index);

    binarioNaTela(indexFileName);
}

// 6th task -> SELECT WHERE Campo = Valor
void searchIndexedLine(void)
{
    // Scanning from stdin
    uint32_t codLinha;
    char binFileName[MAX_STRING_SIZE], indexFileName[MAX_STRING_SIZE];
    scanf("%s %s %*s %d", binFileName, indexFileName, &codLinha);

    // Opening files
    FILE *bin = NULL, *index = NULL;
    _openLineFiles(bin, binFileName, "rb", index, indexFileName, "rb");

    // Searching inside our index for the register 
    Result result = searchRegister(codLinha, NULL/*INDEX*/);
    fclose(index);

    if (result == NOT_FOUND) {
        printf("Registro inexistente.\n");
        fclose(bin);
        exit(0);
    }

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

// 8th task -> INSERT N REGISTERS INTO END
void insertNewLinesThenIndex(void)
{
    // Getting stdin data for number of new registers and filename
    int nRegisters = 0;
    char binFileName[MAX_STRING_SIZE], indexFileName[MAX_STRING_SIZE];
    scanf("%s %s %d", binFileName, indexFileName, &nRegisters);

    FILE *bin = NULL, *index = NULL;
    _openLineFiles(bin, binFileName, "r+b", index, indexFileName, "r+b");

    // Setting file as INCONSISTENT
    setLineFileStatus(bin, STATUS_INCONSISTENT);

    // Writing all CSV Lines into BIN file
    Line *l = newLine();
    fseek(bin, 0, SEEK_END);
    while (nRegisters--)
    {
        // Getting register from CLI
        updateLine(l, NULL, CLI);
        int codLinha = getLineIndex(l);

        // Checking if codLinha is not already in use
        if (searchRegister(codLinha, NULL /*B-TREE*/) != FOUND) {
            // Inserting index into B Tree
            if (!logicallyRemoved(l))
                insertRegister(codLinha);

            // Writing data to file
            writeLine(l, bin, BIN);
        }
    }
    freeLine(l);
    fclose(index);

    // Finishing program. Freeing all memory
    setLineFileStatus(bin, STATUS_CONSISTENT);
    fclose(bin);

    // Proof that our program worked correctly
    binarioNaTela(indexFileName);
}
