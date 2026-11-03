#ifndef TABLA_SIMPLEX_H
#define TABLA_SIMPLEX_H

#include "problema.h"

// Cantidad máxima de filas
#define MAX_FILAS (MAX_RESTRICCIONES + 1)

//Cantidad máxima de columnas
#define MAX_COLUMNAS (MAX_VARIABLES + MAX_RESTRICCIONES + 2)

// Estructura que representa la tabla de Simplex
typedef struct {
    // Matriz que guarda los coeficientes 
    double valores[MAX_FILAS][MAX_COLUMNAS];
    // Guarda los índices de las columnas correspondientes a las variables básicas
    int variables_basicas[MAX_RESTRICCIONES];
    int num_filas;
    int num_columnas;
    int num_variables;
    int num_restricciones;
    // Índice de la columna que guarda los recursos
    int columna_recursos;
} TablaSimplex;

void construir_tabla_inicial(
    Problema *problema,
    TablaSimplex *tabla
);

void mostrar_tabla(TablaSimplex *tabla);

#endif