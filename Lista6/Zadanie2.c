#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

//iteratoor do obsługi wątków
int iterator = 0;
//blokada do obsługi wątków
pthread_mutex_t multiplicationMutex;

typedef struct {
  int **firstMatrix;
  int **secondMatrix;
  int **resultMatrix;
  int matrixSize;
} ex2Structure;

void fullfillMatrix(int **matrix, int size) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      matrix[i][j] = rand()%2;
    }
  }
}

void printMatrix(int **matrix, int size) {
  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
      printf("%d ", matrix[i][j]);
    }
    printf("\n");
  }
}

void *multiplication(void *x) {
  ex2Structure *v = (ex2Structure *) x;
  while (1) {
    pthread_mutex_lock(&multiplicationMutex);
    int i = iterator;
    iterator++;
    pthread_mutex_unlock(&multiplicationMutex);
    if (i >= v->matrixSize) {
      break;
    }
    for (int j = 0; j < v->matrixSize; j++) {
      for (int k = 0; k < v->matrixSize; k++) {
        v->resultMatrix[i][j] = v->resultMatrix[i][j] || (v->firstMatrix[i][k] && v->secondMatrix[k][j]);
        if (v->resultMatrix[i][j] == 1) {
          break;
        }
      }
    }
  }
  pthread_exit(NULL);
}

int main(int argc, char const *argv[]) {
  srand(time(NULL));
  pthread_mutex_init(&multiplicationMutex, NULL);
  int size = 0;
  int threads = 0;
  printf("%s\n", "Enter size of matrix.");
  scanf("%d", &size);
  printf("%s\n", "Enter number of threads.");
  scanf("%d", &threads);
  printf("Size = %d\n", size);
  printf("Threads = %d\n", threads);
  pthread_t t[threads];
  int **firstMatrix = malloc(size*sizeof(int *));
  int **secondMatrix = malloc(size*sizeof(int *));
  int **resultMatrix = malloc(size*sizeof(int *));
  for (int i = 0; i < size; i++) {
    firstMatrix[i] = malloc(size*sizeof(int));
    secondMatrix[i] = malloc(size*sizeof(int));
    resultMatrix[i] = malloc(size*sizeof(int));
  }
  fullfillMatrix(firstMatrix, size);
  fullfillMatrix(secondMatrix, size);

  ex2Structure *v = malloc(sizeof(ex2Structure));
  v->firstMatrix = firstMatrix;
  v->secondMatrix = secondMatrix;
  v->resultMatrix = resultMatrix;
  v->matrixSize = size;

  for (int i = 0; i < threads; i++) {
    pthread_create(&t[i], NULL, multiplication, (void *) v);
  }

  for (int i = 0; i < threads; i++) {
    pthread_join(t[i], NULL);
  }

  printf("\n%s\n", "First Matrix");
  printMatrix(firstMatrix, size);
  printf("\n%s\n", "Second Matrix");
  printMatrix(secondMatrix, size);\
  printf("\n%s\n", "Result Matrix");
  printMatrix(resultMatrix, size);
  printf("\n");

  pthread_mutex_destroy(&multiplicationMutex);
  for (int i = 0; i < size; i++) {
    free(firstMatrix[i]);
    free(secondMatrix[i]);
    free(resultMatrix[i]);
  }
  free(firstMatrix);
  free(secondMatrix);
  free(resultMatrix);
}
