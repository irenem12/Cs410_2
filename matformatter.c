#include <stdlib.h>
#include <stdio.h>
#include "helper.h"

int *matrix;
void transpose(int rolNum, int colNum);

//print transpose of of matrice
void transpose(int rolNum, int colNum) {
  int i;
  int j;
    for (i = 0; i < colNum; i++) {
        for (j = 0; j < rolNum; j++) {
            printf("%d", matrix[j * colNum + i]);
            //print seperator if it is not the last column
            if (j < rolNum) {
                printf(" ");
            }
        }
        printf("\n");
    }
}

int main(int argc, char **argv) {
    matrix = (int *)malloc(sizeof(int) * 100 * 100);
    int rowCol[2];
    if(readMatrix(matrix, rowCol, 0)) exit(EXIT_FAILURE);
    transpose(rowCol[0], rowCol[1]);
    return EXIT_SUCCESS;
}
