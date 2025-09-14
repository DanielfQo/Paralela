// g++ -O2 prueba3.cpp -o prueba3

#include <iostream>
#include <chrono>
#include <cstdlib>
#include <algorithm>

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

void multiplicacionPorBloques(int** A, int** B, int** C, int n, int bs) {
    for (int ii = 0; ii < n; ii += bs) {
        for (int jj = 0; jj < n; jj += bs) {
            for (int kk = 0; kk < n; kk += bs) {
                for (int i = ii; i < min(ii + bs, n); i++) {
                    for (int j = jj; j < min(jj + bs, n); j++) {
                        for (int k = kk; k < min(kk + bs, n); k++) {
                            C[i][j] += A[i][k] * B[k][j];
                        }
                    }
                }
            }
        }
    }
}


int** crearMatriz(int n) {
    int* data = new int[n * n];
    int** M = new int*[n];
    for (int i = 0; i < n; i++) {
        M[i] = &data[i * n];
    }
    return M;
}


void liberarMatriz(int** M) {
    delete[] M[0]; 
    delete[] M;   
}

void inicializarMatrices(int** A, int** B, int** C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A[i][j] = 1;
            B[i][j] = 1;
            C[i][j] = 0;
        }
    }
}

int main() {
    int tamanos[] = {100, 500, 1000, 2000, 2100, 2200};
    int numTamanos = sizeof(tamanos) / sizeof(tamanos[0]);
    int repeticiones = 10;
    int bloque = 64; // tamaño bloque

    for (int t = 0; t < numTamanos; t++) {
        int n = tamanos[t];
        cout << "Matriz " << n << "x" << n << "-----" << endl;

        long long sumaClasica = 0;
        long long sumaBloques = 0;

        for (int r = 0; r < repeticiones; r++) {
            int** A = crearMatriz(n);
            int** B = crearMatriz(n);
            int** C = crearMatriz(n);
            inicializarMatrices(A, B, C, n);

            auto start = high_resolution_clock::now();
            multClasica(A, B, C, n);
            auto end = high_resolution_clock::now();
            sumaClasica += duration_cast<milliseconds>(end - start).count();

            liberarMatriz(A);
            liberarMatriz(B);
            liberarMatriz(C);

            A = crearMatriz(n);
            B = crearMatriz(n);
            C = crearMatriz(n);
            inicializarMatrices(A, B, C, n);

            start = high_resolution_clock::now();
            multiplicacionPorBloques(A, B, C, n, bloque);
            end = high_resolution_clock::now();
            sumaBloques += duration_cast<milliseconds>(end - start).count();

            liberarMatriz(A);
            liberarMatriz(B);
            liberarMatriz(C);
        }

        cout << "Promedio Clasica: " << (sumaClasica / repeticiones) << " ms" << endl;
        cout << "Promedio Bloques (" << bloque << "): " 
             << (sumaBloques / repeticiones) << " ms" << endl;
    }

    return 0;
}
