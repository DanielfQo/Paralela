/*Implementa la multiplicación matriz-vector utilizando una distribución por bloques de submatrices de la matriz.
Asume que los vectores están distribuidos entre los procesos de la diagonal.
Una vez más, puedes hacer que el proceso 0 lea la matriz y agregue las submatrices antes de enviarlas a los procesos.
Asume que comm_sz es un cuadrado perfecto y que comm_sz divide exactamente el orden de la matriz.*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int my_rank, comm_sz;
    int n = 4;  
    int *Matriz, *Vector, *Result;
    int *local_Matriz, *local_Vector, *local_Result;
    

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (n % comm_sz != 0) {
        if (my_rank == 0) 
            printf("n debe ser divisible\n");
        MPI_Finalize();
        return 0;
    }

    int local_n = n / comm_sz;

    Matriz = malloc(n * n * sizeof(int));
    Vector = malloc(n * sizeof(int));
    Result = calloc(n, sizeof(int));

    local_Matriz = malloc(local_n * n * sizeof(int));
    local_Vector = malloc(n * sizeof(int));
    local_Result = calloc(local_n, sizeof(int));

    if (my_rank == 0) {
        for (int i = 0; i < n; i++) {
            Vector[i] = i + 1;

            for (int j = 0; j < n; j++) {
                Matriz[i*n + j] = i + j + 1;

            }
        }
    }

    MPI_Scatter(Matriz, local_n * n, MPI_INT, local_Matriz, local_n * n, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Bcast(Vector, n, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < local_n; i++) {
        for (int j = 0; j < n; j++) {
            local_Result[i] += local_Matriz[i*n + j] * Vector[j];

        }
    }

    if (my_rank == 0) {
        for (int i = 0; i < local_n; i++) 
            Result[i] = local_Result[i];

        for (int src = 1; src < comm_sz; src++) {
            MPI_Recv(Result + src*local_n, local_n, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }

        printf("Resultado y = [ ");

        for (int i = 0; i < n; i++) 
            printf("%d ", Result[i]);

        printf("]\n");
    } else {
        MPI_Send(local_Result, local_n, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    free(Matriz); 
    free(Vector);
    free(Result);
    free(local_Matriz); 
    free(local_Vector); 

    free(local_Result);

    MPI_Finalize();
    return 0;
}
