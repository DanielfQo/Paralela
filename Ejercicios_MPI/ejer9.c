/*Escribe un programa que pueda usarse para determinar el costo de cambiar la distribución de una estructura de datos distribuida.
¿Cuánto tiempo toma cambiar de una distribución por bloques de un vector a una distribución cíclica?
¿Cuánto toma la redistribución inversa?*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int main() {
    int rank, comm_sz;
    int N = 100000;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    int base = N / comm_sz;
    int resid = N % comm_sz;
    int local_n;

    if(rank < resid){
        local_n = base + 1;
    } else {
        local_n = base;
    }

    int *local_block = malloc(local_n * sizeof(int));

    int start_idx;

    if(rank < resid)
        start_idx = rank * (base + 1);
    else
        start_idx = resid * (base + 1) + (rank - resid) * base;

    for (int i = 0; i < local_n; i++) {
        local_block[i] = start_idx + i;
    }

    int cyclic_n = (N + comm_sz - 1) / comm_sz;
    int *local_cyclic = malloc(cyclic_n * sizeof(int));
    int actual_cyclic = 0;

    MPI_Barrier(MPI_COMM_WORLD);

    double t1 = MPI_Wtime();

    for (int p = 0; p < comm_sz; p++) {
        int send_start;
        int send_count;
        if (p < resid) {
            send_start = p * (base + 1);
            send_count = base + 1;
        } else {
            send_start = resid * (base + 1) + (p - resid) * base;
            send_count = base;
        }

        for (int i = 0; i < send_count; i++) {
            if ((send_start + i) % comm_sz == rank) {
                local_cyclic[actual_cyclic++] = send_start + i;
            }
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    double t2 = MPI_Wtime();

    if (rank == 0) 
        printf("Tiempo bloque a ciclico: %f s\n", t2 - t1);

    int *local_block2 = malloc(local_n * sizeof(int));
    int idx = 0;

    MPI_Barrier(MPI_COMM_WORLD);

    double t3 = MPI_Wtime();

    for (int i = 0; i < cyclic_n; i++) {
        int global_idx = local_cyclic[i];
        int owner = global_idx / base;
        if (owner >= resid) {
            owner = (global_idx - resid) / base + resid;
        }
        if (owner == rank) {
            local_block2[idx++] = global_idx;
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    double t4 = MPI_Wtime();

    if (rank == 0) printf("Tiempo ciclico a bloque: %f s\n", t4 - t3);

    free(local_block);
    free(local_cyclic);
    free(local_block2);

    MPI_Finalize();
    return 0;
}

// Si ejecutarmos varias veces podemos ver resultados diferentes
