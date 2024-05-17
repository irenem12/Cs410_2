#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>

#include "helper.h"

int main(int argc, char **argv) {
    int result, shmid, currentRow, currentCol,
        matrix1Col, matrix2Col, resultCol,
        resultBase, matrix2Base, matrix1Index, matrix2Index;

    int *addr;

    result = 0;

    //parse input
    shmid = atoi(argv[2]);
    currentRow = atoi(argv[3]);
    currentCol = atoi(argv[4]);
    matrix1Col = atoi(argv[5]);
    matrix2Col = atoi(argv[6]);
    resultCol = atoi(argv[7]);
    resultBase = atoi(argv[8]);
    matrix2Base = atoi(argv[9]);



    //get shared memory
    addr = shmat(shmid, NULL, 0);
    if (addr == (void *) -1) {
        printf("Error in attaching share memory\n");
        exit(EXIT_FAILURE);
    }

    //compute result

    //starting location of each matrix
    matrix1Index = currentRow * matrix1Col;
    matrix2Index = matrix2Base + currentCol;

    //has matrix1Col/matrix2Row number of arguments
    int i;
    for (i = 0; i < matrix1Col; i++) {
        //increment to result
        result += addr[matrix1Index] * addr[matrix2Index];
        //to move to the next col for matrix1, add 1
        //to move to the next row for matrix2, add the columnsize
        matrix1Index++;
        matrix2Index += matrix2Col;
    }


    //write result to the right location in shared memory
    //which would be the base memory of where matrix3 start + location
    addr[resultBase + currentRow * resultCol + currentCol] = result;



    //detach shared memory segment
    shmdt(addr);


    exit(EXIT_SUCCESS);
}
