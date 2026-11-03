#include <stdio.h>

#include "problema.h"
#include "tabla.h"
#include "simplex.h"

int main() {

    Problema problema = {0};

    problema.num_variables = 3;
    problema.num_restricciones = 4;
    problema.maximizar = 1;

    // Funcion objetivo
    problema.funcion_objetivo[0] = 60;
    problema.funcion_objetivo[1] = 35;
    problema.funcion_objetivo[2] = 20;

    // Restriccion 1
    problema.restricciones[0][0] = 8;
    problema.restricciones[0][1] = 6;
    problema.restricciones[0][2] = 1;
    problema.lado_derecho[0] = 48;

    // Restriccion 2
    problema.restricciones[1][0] = 4;
    problema.restricciones[1][1] = 2;
    problema.restricciones[1][2] = 1.5;
    problema.lado_derecho[1] = 20;

    // Restriccion 3
    problema.restricciones[2][0] = 2;
    problema.restricciones[2][1] = 1.5;
    problema.restricciones[2][2] = 0.5;
    problema.lado_derecho[2] = 8;

    // Restriccion 4
    problema.restricciones[3][0] = 0;
    problema.restricciones[3][1] = 2;
    problema.restricciones[3][2] = 0;
    problema.lado_derecho[3] = 5;


    // Resolver.
    ResultadoSimplex resultado = resolver_simplex(&problema);

    // Mostrar todas las tablas.
    for (int i = 0; i < resultado.cantidad_tablas; i++) {

        printf("\nTABLA %d\n", i);

        mostrar_tabla(&resultado.historial[i]);
    }

    switch (resultado.estado) {

        case SIMPLEX_OPTIMO: {

            TablaSimplex *tabla_final =
                &resultado.historial[resultado.cantidad_tablas - 1];

            double solucion[MAX_VARIABLES];
            double holguras[MAX_RESTRICCIONES];
            double valor_objetivo;

            obtener_solucion(
                &problema,
                tabla_final,
                solucion,
                holguras,
                &valor_objetivo
            );

            printf("\nPRIMERA SOLUCION OPTIMA\n");

            printf("\nVariables de decision:\n");

            for (int i = 0; i < problema.num_variables; i++) {
                printf("x%d = %.2f\n", i + 1, solucion[i]);
            }

            printf("\nVariables de holgura:\n");

            for (int i = 0; i < problema.num_restricciones; i++) {
                printf("s%d = %.2f\n", i + 1, holguras[i]);
            }

            printf("\nValor de Z = %.2f\n", valor_objetivo);

            // Buscar una segunda solucion optima.
            TablaSimplex segunda_tabla;

            double segunda_solucion[MAX_VARIABLES];
            double segundo_valor_objetivo;

            int hay_multiples = obtener_segunda_solucion(
                &problema,
                tabla_final,
                &segunda_tabla,
                segunda_solucion,
                &segundo_valor_objetivo
            );

            if (hay_multiples) {

                printf("\n====================================\n");
                printf("SE DETECTARON SOLUCIONES MULTIPLES\n");
                printf("====================================\n");

                printf("\nSEGUNDA TABLA FINAL\n");

                mostrar_tabla(&segunda_tabla);

                printf("\nSEGUNDA SOLUCION OPTIMA\n");

                for (int i = 0; i < problema.num_variables; i++) {
                    printf("x%d = %.2f\n", i + 1, segunda_solucion[i]);
                }

                printf("\nValor de Z = %.2f\n", segundo_valor_objetivo);

                double soluciones[3][MAX_VARIABLES];

                soluciones_adicionales(
                    &problema,
                    solucion,
                    segunda_solucion,
                    soluciones
                );

                printf("\nTRES SOLUCIONES OPTIMAS ADICIONALES\n");

                for (int i = 0; i < 3; i++) {

                    printf("\nSolucion adicional %d:\n", i + 1);

                    for (int j = 0; j < problema.num_variables; j++) {
                        printf("x%d = %.2f\n", j + 1, soluciones[i][j]);
                    }

                    double z = 0;

                    for (int j = 0; j < problema.num_variables; j++) {
                        z += problema.funcion_objetivo[j] *
                            soluciones[i][j];
                    }

                    printf("Z = %.2f\n", z);
                }

            } else {

                printf("\nNo se encontraron soluciones optimas multiples.\n");
            }

            break;
        }

        case SIMPLEX_NO_ACOTADO:
            printf("Problema no acotado.\n");
            break;

        case SIMPLEX_LIMITE_ITERACIONES:
            printf("Se alcanzo el limite de iteraciones.\n");
            break;
        
        case SIMPLEX_CICLAJE:
            printf("Se detecto ciclaje en el metodo Simplex.\n");
            break;

        case SIMPLEX_ERROR:
            printf("Ocurrio un error.\n");
            break;
    }

    printf("\nIteraciones: %d\n", resultado.iteraciones);

    liberar_memoria(&resultado);

    return 0;
}