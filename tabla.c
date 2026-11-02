#include <stdio.h>
#include <string.h>

#include "tabla.h"

void construir_tabla_inicial(Problema *problema, TablaSimplex *tabla) {

    int n = problema->num_variables;
    int m = problema->num_restricciones;

    memset(tabla, 0, sizeof(TablaSimplex));

    tabla->num_filas = m + 1;
    tabla->num_columnas = n + m + 2;
    tabla->columna_recursos = tabla->num_columnas - 1;

    tabla->num_variables = n;
    tabla->num_restricciones = m;

    tabla->valores[0][0] = 1;

    for (int j = 0; j < n; j++) {
        tabla->valores[0][j + 1] = -problema->funcion_objetivo[j];
    }

    for (int i = 0; i < m; i++) {

        int fila = i + 1;

        for (int j = 0; j < n; j++) {
            tabla->valores[fila][j + 1] = problema->restricciones[i][j];
        }

        int columna_holgura = n + i + 1;
        tabla->valores[fila][columna_holgura] = 1;
        tabla->valores[fila][tabla->columna_recursos] = problema->lado_derecho[i];
        tabla->variables_basicas[i] = columna_holgura;
    }
}

void mostrar_tabla(TablaSimplex *tabla) {

    int n = tabla->num_variables;
    int m = tabla->num_restricciones;

    printf("%8s", "Z");

    for (int j = 1; j <= n; j++) {
        printf("%8s%d", "x", j);
    }

    for (int j = 1; j <= m; j++) {
        printf("%8s%d", "s", j);
    }

    printf("%10s\n", "RHS");

    for (int i = 0; i < tabla->num_filas; i++) {

        for (int j = 0; j < tabla->num_columnas; j++) {

            printf("%8.2f", tabla->valores[i][j]);

        }

        printf("\n");
    }

    printf("\n");
}