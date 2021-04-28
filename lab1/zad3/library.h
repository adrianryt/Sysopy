
typedef struct block {
    char **rows;
    int rowsNumber;
}block;

typedef struct pair {
    char *a_name;
    char *b_name;
    int rows;
}pair;

block *allFilesMerge(pair *pairs, int numberOfPairs);
FILE *mergeTwoFiles(pair * pairs, int i);
pair *fill_pairs_with_data(int numberOfFiles, char ** FileNames);
int numberOfRowsInBlock(block block);
void deleteBlock(int idx, block *mainArr);
void deleteRowFromBlock(int row_idx,int block_idx, block *mainArr);
void listMainTable(block *mainArr, int numberOfPairs);