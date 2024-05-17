#include <limits.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include "helper.h"

int readMatrix(int *matrix, int rowCol[2], int breakAtBlankLine) {

    int colNum = 0;
    int *matrixIter = matrix;
    int lineCount = 0;
    char buf[LINE_MAX] = "";


    int isFirstLine = 1;
    //keep getting line until end of file
    while (fgets(buf, LINE_MAX, stdin) != NULL) {
        //if we are reading 2 matrices, and this is the first matrix, we need to stop reading at a blank line
        if (breakAtBlankLine && strcmp(buf, "\n") == 0) break;
        int col = 0;

        char *token;

        //strtok routine breaks first argument seperate by the second argument
        /* get the first token */
        token = strtok(buf, " ");
        int val;
        /* walk through other tokens */
        while(token) {
            if (token[0] == '\n') {
                break;
            }
            val = atoi(token);

            //if atoi cannot convet token to a number, val would 0
            // also needs to check for if token is 0 before
            if(!val && token[0] != '0') {
                printf("%s", token);
                printf("Input must be number. \n");
                return EXIT_FAILURE;
            }

            //construct the matrix
            *matrixIter = val;
            matrixIter++;
            col++;

            //Go to the next token
            token = strtok(NULL, " ");
        }

        //record number of rows for the first line for the purpose of error checking
        if (isFirstLine) {
            colNum = col;
        }
        
        //if the line has different length than the first line, matrix is not valid
        if (!isFirstLine && col != colNum) {
            printf("Column numbers in each row must be the same. \n");
            return EXIT_FAILURE;
        }

        //set isfirstLine to false after reading the first line
        isFirstLine = 0;

        lineCount++;
    }

    // false if no matrice
    if (!lineCount) {
        printf("No input matrix. \n");
        return EXIT_FAILURE;
    }

    //return row and col number
    rowCol[0] = lineCount;
    rowCol[1] = colNum;
    return EXIT_SUCCESS;
}

//itoa modified from geeksforgeeks.com: how to implement your own itoa

//reverse the chars in the buffer
void reverse(char *str, int length) 
{ 
    int start = 0; 
    int end = length-1; 
    char temp;
    while (start < end) 
    { 
        temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++; 
        end--; 
    } 
}

char* itoa(int val, char *buffer, int base) {

    int i = 0;

    //if the value is 0 return 0 as the output. otherwise it would return an empty string
    if(val == 0) {
        buffer[i++] = '0';
        buffer[i] = '\0';
        return buffer;
    }

    //kee dividing the number by base, write the mod to the buffer
    while(val != 0) {
        int rem = val % base;
        //convert the decimal to hex
        buffer[i++] = (rem > 9) ? (rem-10) + 'a' : rem + '0';
        val = val/base;
    }

    //reverse the buffer, little endian -> big endian
    reverse(buffer, i);

    //add \0 to ends it
    buffer[i] = '\0';
    return buffer;

}
  