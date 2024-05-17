#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include "helper.h"

#include <errno.h>

char* itoa(int num, char* str, int base);
void reverse(char str[], int length);

int *matrix1, *matrix2;
int rowCol1[2], rowCol2[2];


int main(int argc, char **argv) {

    int resultRow, resultCol, shmid, shmSize, resultBase, pidCounter;
    int *addr, *addrIter;
    pid_t pid;


    matrix1 = (int *)malloc(sizeof(int) * 1000 * 1000);
    matrix2 = (int *)malloc(sizeof(int) * 1000 * 1000);

    //read matrix
    if(readMatrix(matrix1, rowCol1, 1)) exit(EXIT_FAILURE);
    if(readMatrix(matrix2, rowCol2, 0)) exit(EXIT_FAILURE);

    //check if two matrices can multiply
    if (rowCol1[1] != rowCol2[0]) {
        printf("The number of columns in the first matrix must be equal to the number of rows in the second matrix\n");
        exit(EXIT_FAILURE);
    }


    //dimension of the result matrix: row would be the same as matrix1, column same as matrix 2
    resultRow = rowCol1[0];
    resultCol = rowCol2[1];

    //calculate total size to reserve shared memory
    shmSize = sizeof(int) * (resultCol * resultRow + rowCol1[0] * rowCol1[1] + rowCol2[0] * rowCol2[1]);

    //get identifier of the of the shared memory segment
    shmid = shmget(IPC_PRIVATE, shmSize, IPC_CREAT | 0600);

    if (shmid == -1) {
        printf("Error in creating share memory id\n");
        printf("errno: %d \n", errno);
        exit(EXIT_FAILURE);
    }

    //attach shared memory into an address space
    addr = shmat(shmid, NULL, 0);

    if (addr == (void *)-1) {
        printf("Error in attaching share memory\n");
        exit(EXIT_FAILURE);
    }


    //write matrix1, matrix2 to shared memory
    addrIter = addr;
    int i;
    int j;

    for (i = 0; i < rowCol1[0]; i++) {
        for (j = 0; j < rowCol1[1]; j++) {
            *addrIter++ = matrix1[i * rowCol1[1] + j];
        }
    }

    for (i = 0; i < rowCol2[0]; i++) {
        for (j = 0; j < rowCol2[1]; j++) {
            *addrIter++ = matrix2[i * rowCol2[1] + j];
        }
    }


    //where result matrix start in shared memory: which is size of matrix1 + matrix2
    resultBase = rowCol1[0] * rowCol1[1] + rowCol2[0] * rowCol2[1];

    //use to store the pids of all the children, useful when waiting for them to end

    pidCounter = 0;


    //fork child
    pid_t pids[resultRow * resultCol];

    for (i = 0; i < resultRow; i++) {
        for (j = 0; j < resultCol; j++) {
            pid = fork();
            if (pid == 0) {
                //CHILDREN CODE

                //convert int to char to use as arguments
                char *buffer = malloc((64 * sizeof(char)));
                char *currentRow = itoa(i, buffer, 10);
                buffer = malloc((64 * sizeof(char)));
                char *currentCol = itoa(j, buffer, 10);
                buffer = malloc((64 * sizeof(char)));
                char *matrix1Col = itoa(rowCol1[1], buffer, 10);
                buffer = malloc((64 * sizeof(char)));
                char *resultBaseString = itoa(resultBase, buffer, 10);
                buffer = malloc((64 * sizeof(char)));
                char *shmidString = itoa(shmid, buffer, 10);
                buffer = malloc((64 * sizeof(char)));
                char *resultColString = itoa(resultCol, buffer, 10);
                buffer = malloc((64 * sizeof(char)));
                char *matrix2Base = itoa(rowCol1[0] * rowCol1[1], buffer, 10);
                buffer = malloc((64 * sizeof(char)));
                char *matrix2Col = itoa(rowCol2[1], buffer, 10);

                //argument
                char *args_to_multiply[] = {"./multiply", "matmult_p.c", 
                                            shmidString, currentRow, currentCol, 
                                            matrix1Col, matrix2Col, resultColString, 
                                            resultBaseString, matrix2Base,  NULL};

                //execute multiply with the arguments
                if(execvp(args_to_multiply[0], args_to_multiply) == -1) {
                    printf("Error in mapping to multiply \n");
                    printf("errno: %d\n", errno);
                    exit(EXIT_FAILURE);
                }

            } else if (pid > 0) {
                //PARENT CODE
                pids[pidCounter] = pid;
                pidCounter++;
            } else {
                printf("Error in forking\n");
                printf("pid: %d\n", pid);
                printf("errno: %d\n", errno);
                exit(EXIT_FAILURE);
            }
        }
    }

    //wait for children to end
    for (i = 0; i < pidCounter; i++) {
        int wstatus;
        do{
			if (waitpid(pids[i], &wstatus, 0) == -1){
				break;
			}
		}while (!WIFEXITED(wstatus) && !WIFSIGNALED(wstatus));
    }



    //print resultMatrix
    for (i = 0; i < resultRow; i++) {
        for (j = 0; j < resultCol; j++) {
            //location would be offset of where matrix3 starts + currentRow * totalCol + currentCol
            printf("%d", addr[resultBase + i * resultCol + j]);
            //print seperator if it is not the last column
            if (j < resultCol) {
                printf(" ");
            }
        }
        printf("\n");
    }


    //detach shared memory segment
    shmdt(addr);

    //remove share memory when done
    if (shmctl(shmid, IPC_RMID, NULL) < 0) {
        printf("Error when removing share memory\n");
        exit(EXIT_FAILURE);
    }



    return EXIT_SUCCESS;
}
