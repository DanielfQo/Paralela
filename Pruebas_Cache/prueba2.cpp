// g++ -O2 prueba2.cpp -o prueba2

#include <iostream>
#include <chrono>
#include <cstdlib>

using namespace std;
using namespace std::chrono;

void multClasica(int** A, int** B, int** C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = 0;  
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

int** crearMatriz(int n) {
    int** M = new int*[n];
    for (int i = 0; i < n; i++) {
        M[i] = new int[n];
    }
    return M;
}

void liberarMatriz(int** M, int n) {
    for (int i = 0; i < n; i++) {
        delete[] M[i];
    }
    delete[] M;
}

int main() {
    int tamanos[] = {100, 500, 1000, 2000, 2100, 2200};
    int numTamanos = sizeof(tamanos) / sizeof(tamanos[0]);

    for (int t = 0; t < numTamanos; t++) {
        int n = tamanos[t];
        cout << "Matriz " << n << "x" << n << "----" << endl;

        int** A = crearMatriz(n);
        int** B = crearMatriz(n);
        int** C = crearMatriz(n);

        for (int i = 0; i < n; i++) { // inicializacion
            for (int j = 0; j < n; j++) {
                A[i][j] = 1;  
                B[i][j] = 1;
                C[i][j] = 0;
            }
        }

        auto start = high_resolution_clock::now();
        multClasica(A, B, C, n);
        auto end = high_resolution_clock::now();

        auto duracion = duration_cast<milliseconds>(end - start);
        cout << "Tiempo: " << duracion.count() << " ms" << endl << endl;

        liberarMatriz(A, n);
        liberarMatriz(B, n);
        liberarMatriz(C, n);
    }

    return 0;
}
