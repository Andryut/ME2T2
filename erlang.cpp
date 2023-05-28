#include <iostream>
#include <cmath>

using namespace std;

double factorial(int n) {
    if (n == 0) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

double erlangC(double lambda, double mu, int m) {
    double rho = lambda / (mu * m);
    double numerador = (1 / factorial(m)) * pow(lambda / mu, m) * (1 / (1 - rho));
    double sumatoria = 0;
    for (int i = 1; i < m; i++) {
        sumatoria += pow(lambda / mu, i) / factorial(i);
    }
    double denominador = numerador + sumatoria + 1;
    double CErlang = numerador / denominador;
    return CErlang;
}