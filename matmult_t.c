#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <sys/time.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "helper.h"

int *matrix1, *matrix2, *resultMatrix;
int rowCol1[2], rowCol2[2];
int resultRow, resultCol;
int thr_id;
int row;
int col;
int threadsneeded;
int resultRow;
int resultCol;

#define MAX_THREAD_COUNT 1000

//Set by the signal handler
static volatile int expired;

// The contexts
static ucontext_t uc[MAX_THREAD_COUNT];

// keep track of number of switches
static int switches;

//to multiply each row and column together
void multiply(int row, int col);
//to create each thread
void my_thr_create(void (*func) (int), int thr_id);

void multiply(int row, int col) {
    int ret, matrix1Index, matrix2Index;

    ret = 0;
    matrix1Index = row * rowCol1[1];
    matrix2Index = col;
    int i;
    for (i = 0; i < rowCol1[1]; i++) {
        ret += matrix1[matrix1Index] * matrix2[matrix2Index];
        matrix1Index++;
        matrix2Index += rowCol2[1];
    }

    resultMatrix[row * resultCol + col] = ret;
}


//this does all the multiplication work
static void
f (int n)
{ //n is the thread id
  while (1)
    {
      if(n % resultCol == 0){
        multiply(n/resultCol - 1, resultCol - 1);
      } else {
        multiply((n - (n % resultCol))/resultCol, n % resultCol - 1);
      }

      // Regularly the expire variable must be checked.
      if (expired)
        {
          //if max switches has been reached then the multiplcation is done
          if (switches == (threadsneeded - 1)){
            return;
          }
            
          expired = 0;
          // Switch to the other context, saving the current one.
          switches++;
          swapcontext (&uc[n], &uc[n+1]);
        }
    }
}

//signal handler
void
handler (int signal)
{
  expired = 1;
}

void my_thr_create(void (*func) (int), int thr_id){
  makecontext(&uc[thr_id], (void (*) (void)) f, 1, thr_id);
}

int
main (void)
{ struct sigaction sa;
  struct itimerval it;

  matrix1 = (int *)malloc(sizeof(int) * 1000 * 1000);
  matrix2 = (int *)malloc(sizeof(int) * 1000 * 1000);

    //read matrix
    if(readMatrix(matrix1, rowCol1, 1)) exit(EXIT_FAILURE);
    if(readMatrix(matrix2, rowCol2, 0)) exit(EXIT_FAILURE);

    //check if two matrices can multiply
    if (rowCol1[1] != rowCol2[0]) {
        printf("The number of columns in the first matrix must be equal to the number of rows in the second matrix");
        exit(EXIT_FAILURE);
    }


    //dimension of the result matrix: row would be the same as matrix1, column same as matrix 2
    resultRow = rowCol1[0];
    resultCol = rowCol2[1];

    //number of threads is given by the size of the return matrix
    int sizeofmatrix = resultRow * resultCol;

    //initialize result matrix
    resultMatrix = malloc(sizeof(int) * sizeofmatrix);

  //number of threads needed is given by the size of the return matrix
  //since each thread is calculating one cell of the return matrix
  threadsneeded = resultRow * resultCol;
  
  char strs[MAX_THREAD_COUNT][8192];

  // Initialize the data structures for the interval timer.
  sa.sa_flags = SA_RESTART;
  sigfillset (&sa.sa_mask);
  sa.sa_handler = handler;
  it.it_interval.tv_sec = 0;
  it.it_interval.tv_usec = 1;
  it.it_value = it.it_interval;

  int sigactionbool = 0;
  int thr_id;
  for (thr_id = 1; thr_id < (threadsneeded + 1); ++thr_id) {
    if (getcontext (&uc[thr_id]) != -1){
      sigactionbool = 1;
    }
  }

  // Install the timer and get the context we can manipulate.
  if (sigaction (SIGPROF, &sa, NULL) < 0
      || setitimer (ITIMER_PROF, &it, NULL) < 0
      || sigactionbool == 0)
    abort ();
  

  for (thr_id = 1; thr_id < (threadsneeded + 1); ++thr_id)
  {
  uc[thr_id].uc_link = &uc[0];
  uc[thr_id].uc_stack.ss_sp = strs[thr_id-1];
  uc[thr_id].uc_stack.ss_size = 8192;
  my_thr_create(f, thr_id);
  }

  //start the first thread running
  swapcontext (&uc[0], &uc[1]);
  putchar ('\n');

  //print resultMatrix
  int i;
  int j;
    for (i = 0; i < resultRow; i++) {
        for (j = 0; j < resultCol; j++) {
            //location would be offset currentRow * totalCol + currentCol
            printf("%d", resultMatrix[i * resultCol + j]);
            //print seperator if it is not the last column
            if (j < resultCol) {
                printf(" ");
            }
        }
        printf("\n");
    }

  return 0;
}