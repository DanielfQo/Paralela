/* Usa MPI para implementar el programa de histograma discutido en la Sección 2.7.1.
   Haz que el proceso 0 lea los datos de entrada y los distribuya entre los procesos.
   Además, haz que el proceso 0 imprima el histograma. */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int Find_bin(float value, float min_hit, int bin_count, float bin_width) {
    int bin = (int)((value - min_hit) / bin_width);
    if (bin >= bin_count)
        bin = bin_count - 1;
    return bin;
}

int main() {
    int rank, size;
    int data_count = 20;
    float data[20] = {1.3, 2.9, 0.4, 0.3, 1.3, 4.4, 1.7, 0.4, 3.2, 0.3, 4.9, 2.4, 3.1, 4.4, 3.9, 0.4, 4.2, 4.5, 4.9, 0.9};

    float min_hist = 0.0, max_hist = 5.0;
    int bin_count = 5;

    float bin_width = (max_hist - min_hist) / bin_count;
    int* bin_counts = NULL;
    int* local_bin_counts = NULL;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double start_time = MPI_Wtime();

    MPI_Bcast(data, data_count, MPI_FLOAT, 0, MPI_COMM_WORLD);

    int local_n = data_count / size;
    int start = rank * local_n;
    int end;
    if (rank == size - 1) {
        end = data_count;  
    } else {
        end = (rank + 1) * local_n;
    }

    local_bin_counts = (int*)calloc(bin_count, sizeof(int));
    if (rank == 0) {
        bin_counts = (int*)calloc(bin_count, sizeof(int));
    }

    for (int i = start; i < end; i++) {
        int bin = Find_bin(data[i], min_hist, bin_count, bin_width);
        local_bin_counts[bin]++;
    }

    MPI_Reduce(local_bin_counts, bin_counts, bin_count, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();

    if (rank == 0) {
        for (int b = 0; b < bin_count; b++) {
            printf("Bin %d: %d\n", b, bin_counts[b]);
        }
        printf("Tiempo de ejecucion: %f segundos\n", end_time - start_time);
        free(bin_counts);
    }

    free(local_bin_counts);
    MPI_Finalize();
    return 0;
}
