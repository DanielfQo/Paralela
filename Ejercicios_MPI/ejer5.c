/*Implementa la multiplicación matriz-vector utilizando una distribución por bloques de columnas de la matriz.
Puedes hacer que el proceso 0 lea la matriz y simplemente use un bucle de envíos para distribuirla entre los procesos.
Asume que la matriz es cuadrada de orden n y que n es divisible exactamente entre comm_sz.*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int comm_sz, my_rank;
    int n = 4;  
    int *Matriz = NULL, *Vector = NULL, *Result = NULL;
    int *local_Matriz = NULL, *local_Result = NULL;
    int local_cols;
    double start_time, end_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (n % comm_sz != 0) {
        if (my_rank == 0)
            printf("El numero de procesos debe dividir exactamente n\n");
        MPI_Finalize();
        return 0;
    }

    local_cols = n / comm_sz;
    local_Matriz = malloc(n * local_cols * sizeof(int));
    local_Result = calloc(n, sizeof(int));

    if (my_rank == 0) {
        Matriz = malloc(n * n * sizeof(int));
        Vector = malloc(n * sizeof(int));
        Result = malloc(n * sizeof(int));

         for (int i = 0; i < n; i++) {
            Vector[i] = i + 1;
            for (int j = 0; j < n; j++) {
                Matriz[i*n + j] = i + j + 1;
            }
        }
    }

    if (my_rank != 0)
        Vector = malloc(n * sizeof(int));

    MPI_Bcast(Vector, n, MPI_INT, 0, MPI_COMM_WORLD);

    if (my_rank == 0) {
        for (int p = 0; p < comm_sz; p++) {
            if (p == 0) {
                for (int i = 0; i < n; i++) {
                    for (int j = 0; j < local_cols; j++) {
                        local_Matriz[i*local_cols + j] = Matriz[i*n + p*local_cols + j];
                    }
                }
            } else {
                int *temp = malloc(n * local_cols * sizeof(int));
                for (int i = 0; i < n; i++) {
                    for (int j = 0; j < local_cols; j++) {
                        temp[i*local_cols + j] = Matriz[i*n + p*local_cols + j];
                    }
                }
                MPI_Send(temp, n*local_cols, MPI_INT, p, 0, MPI_COMM_WORLD);
                free(temp);
            }
        }
    } else {
        MPI_Recv(local_Matriz, n*local_cols, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    MPI_Barrier(MPI_COMM_WORLD);
    start_time = MPI_Wtime();

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < local_cols; j++) {
            local_Result[i] += local_Matriz[i*local_cols + j] * Vector[my_rank*local_cols + j];
        }
    }

    MPI_Reduce(local_Result, Result, n, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);
    end_time = MPI_Wtime();

    if (my_rank == 0) {
        printf("Resultado final = [ ");
        for (int i = 0; i < n; i++)
            printf("%d ", Result[i]);
        printf("]\n");
        printf("Tiempo de ejecucion: %f segundos\n", end_time - start_time);

        free(Matriz);
        free(Vector);
        free(Result);
    } else {
        free(Vector);
    }

    free(local_Matriz);
    free(local_Result);

    MPI_Finalize();
    return 0;
}
