#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    // Inicializa MPI
    MPI_Init(&argc, &argv);

    // Obtiene el número total de procesos
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Obtiene el rank (ID) del proceso actual
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Obtiene el nombre del procesador (nodo)
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Imprime el mensaje
    printf("Hello world desde el procesador %s, rank %d de %d procesos\n",
           processor_name, world_rank, world_size);

    // Finaliza MPI
    MPI_Finalize();
    return 0;
}
