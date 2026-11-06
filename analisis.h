#ifndef ANALISIS_H
#define ANALISIS_H

#include <stddef.h>

#include "problema.h"
#include "simplex.h"

/*  Resultado completo de resolver un problema: el historial de tablas del
    Simplex mas todo lo que se deduce de el (solucion, soluciones multiples,
    degeneracion). Es lo que consumira el generador de LaTeX. */
typedef struct {
    ResultadoSimplex simplex;

    // Validos solo si simplex.estado == SIMPLEX_OPTIMO
    double solucion[MAX_VARIABLES];
    double holguras[MAX_RESTRICCIONES];
    double valor_objetivo;

    // Soluciones multiples (solo si simplex.estado == SIMPLEX_OPTIMO)
    int multiples;
    TablaSimplex segunda_tabla;
    double segunda_solucion[MAX_VARIABLES];
    double segunda_holguras[MAX_RESTRICCIONES];
    double segundo_valor_objetivo;
    double adicionales[3][MAX_VARIABLES];

    /*  Degeneracion: alguna variable basica vale cero en alguna tabla.
        tabla_degenerada es el indice (en simplex.historial) de la primera
        tabla donde ocurre, o -1 si no hay degeneracion. */
    int degenerado;
    int tabla_degenerada;
} Analisis;

/*  Resuelve el problema y completa el analisis.
    Devuelve 0 si todo salio bien, -1 si no hubo memoria.
    Siempre se debe llamar a liberar_analisis() despues si devolvio 0. */
int analizar_problema(Problema *problema, Analisis *analisis);

void liberar_analisis(Analisis *analisis);

/*  Escribe en buffer un resumen legible del resultado (texto plano). */
void resumen_texto(const Problema *problema, const Analisis *analisis,
                   char *buffer, size_t tamano);

#endif
