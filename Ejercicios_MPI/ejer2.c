/*Escribe un programa en MPI que use un método de Monte Carlo para estimar pi.
El proceso 0 debe leer el número total de lanzamientos y difundirlo (broadcast) a los demás procesos.
Usa MPI_Reduce para encontrar la suma global de la variable local number_in_circle, y haz que el proceso 0 imprima el resultado.
Quizá quieras usar long long int para el número de aciertos dentro del círculo y el número de lanzamientos, ya que ambos pueden
necesitar ser muy grandes para obtener una estimación razonable de pi.*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>

int main() {
    int rank, size;
    long long int total_lanzamientos = 100000000;
    long long int local_lanzamientos;  
    long long int local_in_circle = 0 , global_in_circle = 0;

    double x, y;
    double start_time, end_time;

    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Bcast(&total_lanzamientos, 1, MPI_LONG_LONG_INT, 0, MPI_COMM_WORLD);

    local_lanzamientos = total_lanzamientos / size;

    srand(time(NULL) ^ (rank * 1234567));

    start_time = MPI_Wtime();

    for (long long int toss = 0; toss < local_lanzamientos; toss++) {
        x = (double) rand() / RAND_MAX * 2.0 - 1.0;
        y = (double) rand() / RAND_MAX * 2.0 - 1.0;
        double distance_squared = x * x + y * y;
        if (distance_squared <= 1) {
            local_in_circle++;
        }
    }

    MPI_Reduce(&local_in_circle, &global_in_circle, 1, MPI_LONG_LONG_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    end_time = MPI_Wtime();

    if (rank == 0) {
        double pi_estimate = 4.0 * ((double) global_in_circle / (double) total_lanzamientos);
        printf("Estimacion de pi = %f\n", pi_estimate);
        printf("Tiempo de ejecucion: %f segundos\n", end_time - start_time);
    }

    MPI_Finalize();
    return 0;
}
