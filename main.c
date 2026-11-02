#include <stdio.h>

#include "problema.h"
#include "tabla.h"

int main() {

    Problema problema;

    problema.num_variables = 2;
    problema.num_restricciones = 3;
    problema.maximizar = 1;

    // Función objetivo: Z = 3x1 + 5x2
    problema.funcion_objetivo[0] = 3;
    problema.funcion_objetivo[1] = 5;

    // Restricción 1: x1 <= 4
    problema.restricciones[0][0] = 1;
    problema.restricciones[0][1] = 0;
    problema.lado_derecho[0] = 4;

    // Restricción 2: 2x2 <= 12
    problema.restricciones[1][0] = 0;
    problema.restricciones[1][1] = 2;
    problema.lado_derecho[1] = 12;

    // Restricción 3: 3x1 + 2x2 <= 18
    problema.restricciones[2][0] = 3;
    problema.restricciones[2][1] = 2;
    problema.lado_derecho[2] = 18;

    TablaSimplex tabla;

    construir_tabla_inicial(&problema, &tabla);

    printf("TABLA SIMPLEX INICIAL\n");

    mostrar_tabla(&tabla);

    return 0;
}