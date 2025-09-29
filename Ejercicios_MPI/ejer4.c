/*Escribe un programa en MPI que calcule una suma global utilizando un patrón butterfly.
Primero escribe tu programa para el caso especial en el que el tamaño del comunicador (comm_sz) sea una potencia de dos.
*/

#include <stdio.h>
#include <mpi.h>

int main(int argc, char* argv[]) {
    int comm_sz, my_rank;
    int local_value, received_value;
    int mask, partner, global_sum = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    local_value = my_rank;

    for (mask = 1; mask < comm_sz; mask <<= 1) {
        partner = my_rank ^ mask;

        if (partner < comm_sz) {
            if (my_rank < partner) {
                MPI_Send(&local_value, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
                MPI_Recv(&received_value, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            } else {
                MPI_Recv(&received_value, 1, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Send(&local_value, 1, MPI_INT, partner, 0, MPI_COMM_WORLD);
            }

            local_value += received_value;
        }
    }

    if (my_rank == 0) {
        global_sum = local_value;
        printf("Suma global: %d\n", global_sum);
    }

    MPI_Finalize();
    return 0;
}
