#ifndef SIMPLEX_H
#define SIMPLEX_H

#include "tabla.h"

#define MAX_ITERACIONES 1000

/* Tolerancia numerica: valores con |v| < EPSILON se consideran cero.
   Evita que el "ruido" de punto flotante (ej. 1e-17) se tome como pivote. */
#define EPSILON 1e-9

typedef enum {
    SIMPLEX_OPTIMO,
    SIMPLEX_NO_ACOTADO,
    SIMPLEX_LIMITE_ITERACIONES,
    SIMPLEX_ERROR, 
    SIMPLEX_CICLAJE
} EstadoSimplex;

typedef struct {
    EstadoSimplex estado;
    TablaSimplex *historial;
    int cantidad_tablas;
    int iteraciones;
} ResultadoSimplex;

int seleccionar_entrante(
    TablaSimplex *tabla,
    int maximizar
);

int seleccionar_saliente(
    TablaSimplex *tabla,
    int columna_entrante
);

void realizar_pivote(
    TablaSimplex *tabla,
    int fila_pivote,
    int columna_pivote
);

ResultadoSimplex resolver_simplex(Problema *problema);

void liberar_memoria(ResultadoSimplex *resultado);

void obtener_solucion(
    Problema *problema,
    TablaSimplex *tabla,
    double solucion[],
    double holguras[],
    double *valor_objetivo
);


int obtener_segunda_solucion(
    Problema *problema,
    TablaSimplex *tabla,
    TablaSimplex *segunda_tabla,
    double segunda_solucion[],
    double *segundo_valor_objetivo
);

void soluciones_adicionales(
    Problema *problema,
    double primera_solucion[],
    double segunda_solucion[],
    double soluciones[3][MAX_VARIABLES]
);

#endif