#include <stdio.h>
#include <string.h>

#include "tabla.h"

// Construye la tabla inicial de Simplex 
void construir_tabla_inicial(Problema *problema, TablaSimplex *tabla) {

    int n = problema->num_variables;
    int m = problema->num_restricciones;

    // Inicializa toda la estructura TablaSimplex en cero
    memset(tabla, 0, sizeof(TablaSimplex));

    tabla->num_filas = m + 1;
    tabla->num_columnas = n + m + 2;
    tabla->columna_recursos = tabla->num_columnas - 1;
    tabla->num_variables = n;
    tabla->num_restricciones = m;

    // Asigna el coeficiente de Z en la primera fila
    tabla->valores[0][0] = 1;

    // Recorre los coeficientes de la función objetivo y los guarda en la primera fila
    for (int j = 0; j < n; j++) {
        tabla->valores[0][j + 1] = -problema->funcion_objetivo[j];
    }

    // Recorre todas las restricciones del problema
    // i = restricción actual.
    for (int i = 0; i < m; i++) {
        int fila = i + 1;

        // Copia los coeficientes de las variables de decisión de la restricción actual
        for (int j = 0; j < n; j++) {
            tabla->valores[fila][j + 1] = problema->restricciones[i][j];
        }

        // Calcula la columna donde se colocará la variable de holgura
        int columna_holgura = n + i + 1;
        tabla->valores[fila][columna_holgura] = 1;

        // GUarda el valor del lado derecho de la restricción
        tabla->valores[fila][tabla->columna_recursos] = problema->lado_derecho[i];
        tabla->variables_basicas[i] = columna_holgura;
    }
}


// Función que muestra la tabla construida
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