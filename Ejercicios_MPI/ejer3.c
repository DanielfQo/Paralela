/*Escribe un programa en MPI que calcule una suma global con estructura de árbol.
Primero escribe tu programa para el caso especial en el que el tamaño del comunicador (comm_sz) sea una potencia de dos.
Luego, una vez que tengas esta versión funcionando, modifica tu programa para que pueda manejar cualquier valor de comm_sz.*/

#include <stdio.h>
#include <mpi.h>

int main() {
    int comm_sz, my_rank, local_value, received_value, step;
    int global_sum = 0;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    local_value = my_rank;

    int max_pot = 1;
    while (max_pot * 2 <= comm_sz) {
        max_pot *= 2;
    }

    if (my_rank >= max_pot) {
        MPI_Send(&local_value, 1, MPI_INT, my_rank - max_pot, 0, MPI_COMM_WORLD);
    } else if (my_rank + max_pot < comm_sz) {
        MPI_Recv(&received_value, 1, MPI_INT, my_rank + max_pot, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        local_value += received_value;
    }

    for (step = 1; step < max_pot; step *= 2) {
        if (my_rank % (2 * step) == 0) {
            MPI_Recv(&received_value, 1, MPI_INT, my_rank + step, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            local_value += received_value;
        } else if (my_rank % step == 0) {
            MPI_Send(&local_value, 1, MPI_INT, my_rank - step, 0, MPI_COMM_WORLD);
            break;
        }
    }

    if (my_rank == 0) {
        global_sum = local_value;
        printf("Suma Global: %d\n", global_sum);
    }

    MPI_Finalize();
    return 0;
}
