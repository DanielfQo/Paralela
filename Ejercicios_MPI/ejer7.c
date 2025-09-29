/*Un ping-pong es una comunicación en la que se envían dos mensajes: primero del proceso A al proceso B (ping), y luego del proceso B 
de vuelta al proceso A (pong).
Medir bloques de repeticiones de ping-pong es una forma común de estimar el costo de enviar mensajes.
Mide el tiempo de un programa ping-pong usando la función clock de C en tu sistema.
¿Cuánto tiempo debe ejecutarse el código antes de que clock dé un tiempo distinto de cero?
¿Cómo se comparan los tiempos que obtuviste con clock frente a los tiempos medidos con MPI_Wtime?*/

#include <stdio.h>
#include <time.h>
#include <mpi.h>

int main() {
    int rank, size;
    int msj = 123;

    int max_reps = 10000000;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (size != 2) {
        if (rank == 0) printf("Se necesitan 2 procesos.\n");
        MPI_Finalize();
        return 0;
    }

    for (int n_reps = 1; n_reps <= max_reps; n_reps *= 10) {
        double start_mpi = MPI_Wtime();

        for (int i = 0; i < n_reps; i++) {
            if ((i + rank) % 2 == 0) {
                MPI_Send(&msj, 1, MPI_INT, 1 - rank, 0, MPI_COMM_WORLD);
            } else {
                MPI_Recv(&msj, 1, MPI_INT, 1 - rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }

        double end_mpi = MPI_Wtime();

        clock_t start_clock = clock();

        for (int i = 0; i < n_reps; i++) {
            if ((i + rank) % 2 == 0) {
                MPI_Send(&msj, 1, MPI_INT, 1 - rank, 0, MPI_COMM_WORLD);
            } else {
                MPI_Recv(&msj, 1, MPI_INT, 1 - rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            }
        }

        clock_t end_clock = clock();
        double time_clock = (double)(end_clock - start_clock) / CLOCKS_PER_SEC;

        if (rank == 0) {
            printf("n_reps = %d, tiempo MPI_Wtime = %f s, tiempo clock() = %f s\n", n_reps, end_mpi - start_mpi, time_clock);
        }
    }

    MPI_Finalize();
    return 0;
}