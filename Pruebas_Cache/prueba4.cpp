#include <iostream>
#include <chrono>
#include <cstdlib>
#include <algorithm>
#include <string>

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

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Uso: " << argv[0] << " <clasica|bloques>\n";
        return 1;
    }

    int n = 500;       
    int bloque = 32;
    string modo = argv[1];

    cout << "Matriz " << n << "x" << n << " | Algoritmo = " << modo << endl;

    int** A = crearMatriz(n);
    int** B = crearMatriz(n);
    int** C = crearMatriz(n);
    inicializarMatrices(A, B, C, n);

    auto start = high_resolution_clock::now();

    if (modo == "clasica") {
        multClasica(A, B, C, n);
    } else if (modo == "bloques") {
        multiplicacionPorBloques(A, B, C, n, bloque);
    } else {
        cerr << "Algoritmo no válido. Usa 'clasica' o 'bloques'.\n";
        liberarMatriz(A); liberarMatriz(B); liberarMatriz(C);
        return 1;
    }

    auto end = high_resolution_clock::now();
    cout << "Tiempo: " 
         << duration_cast<milliseconds>(end - start).count() 
         << " ms" << endl;

    liberarMatriz(A);
    liberarMatriz(B);
    liberarMatriz(C);

    return 0;
}
