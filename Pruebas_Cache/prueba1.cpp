// g++ -O2 prueba1.cpp -o prueba1

#include <iostream>
#include <chrono>

using namespace std;
using namespace std::chrono;

const int MAX = 10000;
const int REP = 100; 

int main() {
    static int A[MAX][MAX];
    static int x[MAX], y[MAX];

    for (int i = 0; i < MAX; i++) {
        x[i] = 1;
        y[i] = 0;
        for (int j = 0; j < MAX; j++) {
            A[i][j] = 1;
        }
    }

    long long total1 = 0;
    long long total2 = 0;

    for (int r = 0; r < REP; r++) {
        for (int i = 0; i < MAX; i++) y[i] = 0;

        auto start1 = high_resolution_clock::now();
        for (int i = 0; i < MAX; i++) {
            for (int j = 0; j < MAX; j++) {
                y[i] += A[i][j] * x[j];
            }
        }
        auto end1 = high_resolution_clock::now();
        total1 += duration_cast<milliseconds>(end1 - start1).count();
    }

    for (int r = 0; r < REP; r++) {
        for (int i = 0; i < MAX; i++) y[i] = 0;

        auto start2 = high_resolution_clock::now();
        for (int j = 0; j < MAX; j++) {
            for (int i = 0; i < MAX; i++) {
                y[i] += A[i][j] * x[j];
            }
        }
        auto end2 = high_resolution_clock::now();
        total2 += duration_cast<milliseconds>(end2 - start2).count();
    }

    cout << "Promedio acceso por filas: " << (total1 / REP) << " ms" << endl;
    cout << "Promedio acceso por columnas: " << (total2 / REP) << " ms" << endl;

    return 0;
}
