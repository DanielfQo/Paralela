/*El merge sort paralelo comienza con n/comm_sz claves asignadas a cada proceso.
Finaliza con todas las claves almacenadas en el proceso 0 en orden ascendente.
Para lograr esto, utiliza la misma comunicación con estructura de árbol que usamos para implementar una suma global.
Sin embargo, cuando un proceso recibe las claves de otro, combina (merge) las nuevas claves con su lista ya ordenada.

Escribe un programa que implemente merge sort paralelo.
El proceso 0 debe leer n y difundirlo (broadcast) a los demás procesos.
Cada proceso debe usar un generador aleatorio para crear una lista local de n/comm_sz enteros.
Cada proceso debe ordenar su lista local, y el proceso 0 debe recolectar y mostrar las listas locales.
Luego, los procesos deben usar comunicación en árbol para combinar la lista global en el proceso 0, que imprime el resultado.*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int compare_ints(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

int* merge(int *a, int size_a, int *b, int size_b) {
    int *result = (int*)malloc((size_a + size_b) * sizeof(int));
    int i = 0, j = 0, k = 0;
    while (i < size_a && j < size_b) {
        if (a[i] <= b[j]) result[k++] = a[i++];
        else result[k++] = b[j++];
    }
    while (i < size_a) result[k++] = a[i++];
    while (j < size_b) result[k++] = b[j++];
    return result;
}

int main(int argc, char** argv) {
    int rank, comm_sz;

    int n = 15; // num de elementos

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int local_n = n / comm_sz;

    int resid = n % comm_sz;

    if (rank < resid) 
        local_n++;

    int *local_data = (int*)malloc(local_n * sizeof(int));

    srand(time(NULL) + rank);
    for (int i = 0; i < local_n; i++) {
        local_data[i] = rand() % 100;
    }

    qsort(local_data, local_n, sizeof(int), compare_ints);

    if (rank == 0) {
        printf("Listas locales :\n");
    }
    for (int p = 0; p < comm_sz; p++) {
        if (rank == p) {
            printf("Proceso %d: ", rank);
            for (int i = 0; i < local_n; i++) printf("%d ", local_data[i]);
            printf("\n");
            fflush(stdout);
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }

    int step = 1;

    while (step < comm_sz) {
        if (rank % (2*step) == 0) {
            int recv_rank = rank + step;
            if (recv_rank < comm_sz) {
                int recv_n;
                MPI_Recv(&recv_n, 1, MPI_INT, recv_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int *recv_data = (int*)malloc(recv_n * sizeof(int));
                MPI_Recv(recv_data, recv_n, MPI_INT, recv_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                int *merged = merge(local_data, local_n, recv_data, recv_n);
                free(local_data);
                free(recv_data);
                local_data = merged;
                local_n += recv_n;
            }
        } else {
            int dest_rank = rank - step;
            MPI_Send(&local_n, 1, MPI_INT, dest_rank, 0, MPI_COMM_WORLD);
            MPI_Send(local_data, local_n, MPI_INT, dest_rank, 0, MPI_COMM_WORLD);
            
            break;
        }
        step *= 2;
    }

    if (rank == 0) {
        printf("\nLista global ordenada:\n");
        for (int i = 0; i < local_n; i++) printf("%d ", local_data[i]);
        printf("\n");
    }

    free(local_data);
    MPI_Finalize();
    return 0;
}
