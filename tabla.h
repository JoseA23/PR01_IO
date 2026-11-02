#ifndef TABLA_SIMPLEX_H
#define TABLA_SIMPLEX_H

#include "problema.h"

#define MAX_FILAS (MAX_RESTRICCIONES + 1)
#define MAX_COLUMNAS (MAX_VARIABLES + MAX_RESTRICCIONES + 2)

typedef struct {
    double valores[MAX_FILAS][MAX_COLUMNAS];
    int variables_basicas[MAX_RESTRICCIONES];
    int num_filas;
    int num_columnas;
    int num_variables;
    int num_restricciones;
    int columna_recursos;
} TablaSimplex;

void construir_tabla_inicial(
    Problema *problema,
    TablaSimplex *tabla
);

void mostrar_tabla(TablaSimplex *tabla);

#endif