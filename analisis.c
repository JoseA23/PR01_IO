#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "analisis.h"

/* Busca si alguna variable basica vale cero en alguna tabla del historial */
static void detectar_degeneracion(Analisis *analisis) {

    analisis->degenerado = 0;
    analisis->tabla_degenerada = -1;

    for (int k = 0; k < analisis->simplex.cantidad_tablas; k++) {
        TablaSimplex *tabla = &analisis->simplex.historial[k];

        for (int i = 1; i < tabla->num_filas; i++) {
            if (fabs(tabla->valores[i][tabla->columna_recursos]) < EPSILON) {
                analisis->degenerado = 1;
                analisis->tabla_degenerada = k;
                return;
            }
        }
    }
}

int analizar_problema(Problema *problema, Analisis *analisis) {

    memset(analisis, 0, sizeof(Analisis));
    analisis->tabla_degenerada = -1;

    analisis->simplex = resolver_simplex(problema);

    if (analisis->simplex.historial == NULL) {
        return -1;
    }

    detectar_degeneracion(analisis);

    if (analisis->simplex.estado != SIMPLEX_OPTIMO) {
        return 0;
    }

    TablaSimplex *tabla_final =
        &analisis->simplex.historial[analisis->simplex.cantidad_tablas - 1];

    obtener_solucion(problema, tabla_final, analisis->solucion,
                     analisis->holguras, &analisis->valor_objetivo);

    // Si hay soluciones multiples, pivotea una vez mas para hallar otra
    analisis->multiples = obtener_segunda_solucion(
        problema, tabla_final, &analisis->segunda_tabla,
        analisis->segunda_solucion, &analisis->segundo_valor_objetivo);

    if (analisis->multiples) {
        obtener_solucion(problema, &analisis->segunda_tabla,
                         analisis->segunda_solucion, analisis->segunda_holguras,
                         &analisis->segundo_valor_objetivo);

        soluciones_adicionales(problema, analisis->solucion,
                               analisis->segunda_solucion,
                               analisis->adicionales);
    }

    return 0;
}

void liberar_analisis(Analisis *analisis) {
    liberar_memoria(&analisis->simplex);
}


/* ---------------- Resumen en texto plano ---------------- */

static void agregar(char *buffer, size_t tamano, size_t *largo,
                    const char *formato, ...) {

    if (*largo >= tamano) {
        return;
    }

    va_list args;
    va_start(args, formato);
    int escritos = vsnprintf(buffer + *largo, tamano - *largo, formato, args);
    va_end(args);

    if (escritos > 0) {
        *largo += (size_t)escritos;
        if (*largo >= tamano) {
            *largo = tamano - 1;
        }
    }
}

// Nombre de la variable j: el que escribio el usuario o "x<j+1>" por defecto
static const char *nombre_variable(const Problema *problema, int j, char *tmp, size_t n) {
    if (problema->nombres_variables[j][0] != '\0') {
        return problema->nombres_variables[j];
    }
    snprintf(tmp, n, "x%d", j + 1);
    return tmp;
}

static void agregar_solucion(const Problema *problema, char *buffer, size_t tamano,
                             size_t *largo, const double x[], double z) {
    char tmp[16];

    for (int j = 0; j < problema->num_variables; j++) {
        agregar(buffer, tamano, largo, "   %s = %.6g\n",
                nombre_variable(problema, j, tmp, sizeof(tmp)), x[j]);
    }
    agregar(buffer, tamano, largo, "   Z = %.6g\n", z);
}

void resumen_texto(const Problema *problema, const Analisis *analisis,
                   char *buffer, size_t tamano) {

    size_t largo = 0;

    if (tamano == 0) {
        return;
    }
    buffer[0] = '\0';

    agregar(buffer, tamano, &largo, "Problema: %s\n", problema->nombre);
    agregar(buffer, tamano, &largo, "Objetivo: %s\n",
            problema->maximizar ? "Maximizar" : "Minimizar");
    agregar(buffer, tamano, &largo, "Tablas generadas: %d   Iteraciones: %d\n\n",
            analisis->simplex.cantidad_tablas, analisis->simplex.iteraciones);

    switch (analisis->simplex.estado) {

        case SIMPLEX_OPTIMO:
            agregar(buffer, tamano, &largo, "SOLUCION OPTIMA\n");
            agregar_solucion(problema, buffer, tamano, &largo,
                             analisis->solucion, analisis->valor_objetivo);

            agregar(buffer, tamano, &largo, "\nVariables de holgura:\n");
            for (int i = 0; i < problema->num_restricciones; i++) {
                agregar(buffer, tamano, &largo, "   s%d = %.6g\n", i + 1,
                        analisis->holguras[i]);
            }

            if (analisis->multiples) {
                agregar(buffer, tamano, &largo,
                        "\nSOLUCIONES MULTIPLES: hay infinitas soluciones optimas.\n"
                        "Segunda solucion (tras un pivoteo adicional):\n");
                agregar_solucion(problema, buffer, tamano, &largo,
                                 analisis->segunda_solucion,
                                 analisis->segundo_valor_objetivo);

                for (int k = 0; k < 3; k++) {
                    double z = 0;
                    for (int j = 0; j < problema->num_variables; j++) {
                        z += problema->funcion_objetivo[j] * analisis->adicionales[k][j];
                    }
                    agregar(buffer, tamano, &largo, "Solucion adicional %d:\n", k + 1);
                    agregar_solucion(problema, buffer, tamano, &largo,
                                     analisis->adicionales[k], z);
                }
                agregar(buffer, tamano, &largo,
                        "Todas: x = L * (solucion 1) + (1 - L) * (solucion 2), 0 <= L <= 1\n");
            }
            break;

        case SIMPLEX_NO_ACOTADO:
            agregar(buffer, tamano, &largo,
                    "PROBLEMA NO ACOTADO: la funcion objetivo puede mejorar "
                    "indefinidamente.\n");
            break;

        case SIMPLEX_CICLAJE:
            agregar(buffer, tamano, &largo,
                    "Se detecto ciclaje en el metodo Simplex.\n");
            break;

        case SIMPLEX_LIMITE_ITERACIONES:
            agregar(buffer, tamano, &largo,
                    "Se alcanzo el limite de iteraciones (%d).\n", MAX_ITERACIONES);
            break;

        case SIMPLEX_ERROR:
        default:
            agregar(buffer, tamano, &largo, "Ocurrio un error al resolver.\n");
            break;
    }

    if (analisis->degenerado) {
        agregar(buffer, tamano, &largo,
                "\nPROBLEMA DEGENERADO: una variable basica vale 0 en la tabla %d.\n"
                "Los empates de razones se resuelven escogiendo la fila de menor indice.\n",
                analisis->tabla_degenerada);
    }

}
