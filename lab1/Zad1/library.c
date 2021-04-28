#define  _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include "library.h"



block *create_blocks(int n){
    block *result = calloc(n,sizeof(block));
    return result;

}


pair *create_pairs (int n){
    int pairNumber = n/2;
    pair*result = calloc(pairNumber,sizeof(pair));
    return result;
}

int numberOfRowsInBlock( block block){
    return block.rowsNumber;
}

block *allFilesMerge(pair *pairs, int numberOfPairs){ //number of Files must be even
    int n = numberOfPairs; //ilosc blokow
    block *mainArray = NULL;
    mainArray = create_blocks(n); //define main array

    for(int i=0; i< n; i++){
       FILE *merged = mergeTwoFiles(pairs,i);
       rewind(merged);
       char *line = NULL;
       size_t line_size = 0;
       mainArray[i].rowsNumber = pairs[i].rows; //alokuje ile tam lini bedzie w bloku
       mainArray[i].rows = calloc(mainArray[i].rowsNumber,sizeof(char*));
       int j=0;
       while(getline(&line, &line_size, merged) != -1 && j < mainArray[i].rowsNumber){ //proba przepisania pliku do bloku :)
          mainArray[i].rows[j] = line;
          j++;
          line = NULL;
          line_size = 0;
       }
       free(line);
       line = NULL;
       fclose(merged);
    }
    return mainArray;
}

FILE *mergeTwoFiles(pair * pairs, int i){
   FILE *a;
   FILE *b;
   FILE *res;
   a = fopen(pairs[i].a_name,"r");
   b = fopen(pairs[i].b_name,"r");
   res = tmpfile();
   size_t line_file_size = 0; // zapewnia poprawnosc zdefiniowanego rozmiaru
   char *line_file = NULL;
   int number_of_rows = 0;
   int c=1;
   int d=1;
   while(c && d){
        if(getline(&line_file, &line_file_size, a) != -1){
            fprintf(res,"%s",line_file);
            number_of_rows++;
        }
        else{
            c= 0;
        }

        if(getline(&line_file, &line_file_size, b) != -1){
            fprintf(res,"%s",line_file);
            number_of_rows++;
        }
        else{
            d= 0;
        }
    }
    while(c){
        if(getline(&line_file, &line_file_size, a) != -1){
            fprintf(res,"%s",line_file);
            number_of_rows++;
        }
        else{
            c= 0;
        }  
    }

    while(d){
        if(getline(&line_file, &line_file_size, b) != -1){
            fprintf(res,"%s",line_file);
            number_of_rows++;
        }
        else{
            d= 0;
        }
    }
    pairs[i].rows = number_of_rows;
    free(line_file);
    line_file = NULL;
    fclose(a);
    fclose(b);
    return res;
}

pair *fill_pairs_with_data(int numberOfFiles, char ** FileNames){//funkcja wpisuje nazwy 2 plikow do zmergowania
    pair *pairs = create_pairs(numberOfFiles);
    int n = numberOfFiles/2;
    for( int i= 0; i< n; i++){
        pairs[i].a_name = FileNames[i*2];
        pairs[i].b_name = FileNames[i*2 + 1];
    }
    return pairs;
}

void deleteBlock(int idx, block *mainArr){
    if(mainArr[idx].rows == NULL){
        printf("%s", "Wrong index");
        return;
    }

    for(int i = 0; i < mainArr[idx].rowsNumber; i++){ //usuwanie wierszy w bloku
        free(mainArr[idx].rows[i]);
        mainArr[idx].rows[i] = NULL;
    }
    free(mainArr[idx].rows); //usuwanie bloku
    mainArr[idx].rows = NULL;
}

void deleteRowFromBlock(int row_idx,int block_idx, block *mainArr){
    if(mainArr[block_idx].rowsNumber <= row_idx || mainArr[block_idx].rows == NULL){
        return;
    }
    free(mainArr[block_idx].rows[row_idx]);
    mainArr[block_idx].rows[row_idx] = NULL;
}

void listMainTable(block *mainArr, int numberOfPairs){
    for(int i=0; i < numberOfPairs; i++){
        for(int j=0; j<mainArr[i].rowsNumber; j++){
            if(mainArr[i].rows != NULL && mainArr[i].rows[j] != NULL)
            printf("%s",mainArr[i].rows[j]);
        }
    }
}