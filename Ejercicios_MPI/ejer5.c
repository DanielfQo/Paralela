/*Implementa la multiplicación matriz-vector utilizando una distribución por bloques de columnas de la matriz.
Puedes hacer que el proceso 0 lea la matriz y simplemente use un bucle de envíos para distribuirla entre los procesos.
Asume que la matriz es cuadrada de orden n y que n es divisible exactamente entre comm_sz.*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>

int main() {
    int comm_sz, my_rank;
    int n = 4;  
    int *Matriz = NULL, *Vector = NULL, *Result = NULL;
    int *local_Matriz = NULL, *local_Vector = NULL, *local_Result = NULL;
    int q, local_n;

    Matriz = malloc(n * n * sizeof(int));
    Vector = malloc(n * sizeof(int));
    Result = malloc(n * sizeof(int));

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    q = (int)sqrt(comm_sz);
    if (q * q != comm_sz || n % q != 0) {
        if (my_rank == 0)
            printf("comm_sz debe ser cuadrado perfecto y sqrt(comm_sz) debe dividir n\n");
        MPI_Finalize();
        return 0;
    }

    local_n = n / q;
    local_Matriz = malloc(local_n * local_n * sizeof(int));
    local_Vector = malloc(local_n * sizeof(int));
    local_Result = calloc(local_n, sizeof(int));

    if (my_rank == 0) {
        for (int i = 0; i < n; i++) {
            Vector[i] = i + 1;
            for (int j = 0; j < n; j++) {
                Matriz[i*n + j] = i + j + 1;
            }
        }
    }

    MPI_Comm comm;
    MPI_Comm_split(MPI_COMM_WORLD, my_rank / q, my_rank % q, &comm);

    if (my_rank == 0) {
        for (int proc = 0; proc < comm_sz; proc++) {
            int row_block = (proc / q) * local_n;
            int col_block = (proc % q) * local_n;

            if (proc == 0) {

                for (int i = 0; i < local_n; i++) {
                    for (int j = 0; j < local_n; j++) {
                        local_Matriz[i*local_n + j] = Matriz[(row_block+i)*n + col_block+j];

                    }
                }
            } else {

                int *temp = malloc(local_n * local_n * sizeof(int));
                for (int i = 0; i < local_n; i++) {
                    for (int j = 0; j < local_n; j++) {
                        temp[i*local_n + j] = Matriz[(row_block+i)*n + col_block+j];
                    }
                }

                MPI_Send(temp, local_n*local_n, MPI_INT, proc, 0, MPI_COMM_WORLD);
                free(temp);
            }
        }
    } else {

        MPI_Recv(local_Matriz, local_n*local_n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    if ((my_rank / q) == (my_rank % q)) {

        for (int i = 0; i < local_n; i++)
            local_Vector[i] = Vector[(my_rank % q)*local_n + i];
    }
    
    MPI_Bcast(local_Vector, local_n, MPI_INT, my_rank % q, comm);

    for (int i = 0; i < local_n; i++) {
        for (int j = 0; j < local_n; j++) {
            local_Result[i] += local_Matriz[i*local_n + j] * local_Vector[j];
        }
    }

    MPI_Gather(local_Result, local_n, MPI_INT, Result, local_n, MPI_INT, 0, comm);

    if (my_rank == 0) {
        printf("Resultado final = [ ");
        for (int i = 0; i < n; i++)
            printf("%d ", Result[i]);
        printf("]\n");
    }

    free(local_Matriz);
    free(local_Vector);
    free(local_Result);
    if (my_rank == 0) {
        free(Matriz);
        free(Vector);
        free(Result);

    }

    MPI_Comm_free(&comm);
    MPI_Finalize();
    return 0;
}

