/*
Universidad de Pamplona
Facultad de Ingenierías y Arquitectura
Programa de Ingeniería de Sistemas
Asignatura: Fundamentos de Computación paralela y distribuida
Profesor: Jose Orlando Maldonado Bautista
*/


/*
En este programa, se genera una matriz de tamaño 4xN en el proceso maestro y distribuye cada fila de la matriz en un proceso diferente. Cada proceso debe calcular el máximo, el mínimo y el promedio de su fila asignada y retornar los resultados al proceso maestro, quien los debe reunir en un vector de máximos, un vector de mínimos y y un vector de promedios de las filas de la matriz original.

*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h> 
#include <time.h>

double promVector(int v[], int n) {
    int suma = 0;
    for (int i = 0; i < n; i++)
        suma += v[i];
    return suma / (double)(n);
}

int maxVector(int v[], int n) {
    int max = v[0];
    for (int i = 1; i < n; i++)
        if (v[i] > max) max = v[i];
    return max;
}

int minVector(int v[], int n) {
    int min = v[0];
    for (int i = 1; i < n; i++)
        if (v[i] < min) min = v[i];
    return min;
}

void imprimirMatriz(int M[], int fil, int col) {
    for (int i = 0; i < fil; i++) {
        for (int j = 0; j < col; j++)
            printf("%d ", M[i * col + j]);
        printf("\n");
    }
}

void imprimirVector(int V[], int tam) {
    for (int i = 0; i < tam; i++)
        printf("%d ", V[i]);
}

void imprimirVectorReal(double V[], int tam) {
    for (int i = 0; i < tam; i++)
        printf("%f ", V[i]);
}

int main(int argc, char *argv[]) {
    int rank, size, *matriz, *filas;
    MPI_Status estado;
    MPI_Init(&argc, &argv); 
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int fil = 2 * size; //Tamaño de las filas, en este caso es 2 * NP
    int col = 5; //Tamaño de la columna
    int tam_matriz = fil * col; //Tamaño de matriz

    if (rank == 0) {
        srand(time(NULL));
        matriz = (int *)malloc(tam_matriz * sizeof(int));
        for (int i = 0; i < tam_matriz; i++)
            matriz[i] = rand() % 80 + 10;

        printf("Matriz generada por el proceso maestro:\n");
        imprimirMatriz(matriz, fil, col);
        printf("\n");
    }

    int filas_por_proceso = fil / size; //filas por proceso
    filas = (int *)malloc(filas_por_proceso * col * sizeof(int));

    // Distribuir las filas de la matriz a los procesos
    MPI_Scatter(matriz, filas_por_proceso * col, MPI_INT, filas, filas_por_proceso * col, MPI_INT, 0, MPI_COMM_WORLD);

    // Cada proceso calcula el mínimo, el máximo y el promedio de sus filas asignadas
    int min = minVector(filas, filas_por_proceso * col);
    int max = maxVector(filas, filas_por_proceso * col);
    double prom = promVector(filas, filas_por_proceso * col);

    int *minimos = (int *)malloc(size * sizeof(int));
    MPI_Gather(&min, 1, MPI_INT, minimos, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    int *maximos = (int *)malloc(size * sizeof(int));
    MPI_Gather(&max, 1, MPI_INT, maximos, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    double *promedios = (double *)malloc(size * sizeof(double));
    MPI_Gather(&prom, 1, MPI_DOUBLE, promedios, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // El proceso 0 imprime los resultados
    if (rank == 0) {
        printf("Resultados:\n");
        printf("Vector de mínimos:\n");
        imprimirVector(minimos, size);
        printf("\n");

        printf("Vector de máximos:\n");
        imprimirVector(maximos, size);
        printf("\n");

        printf("Vector de promedios:\n");
        imprimirVectorReal(promedios, size);
        printf("\n");
    }

    MPI_Finalize();
}

