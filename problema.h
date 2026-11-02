#ifndef PROBLEMA_H
#define PROBLEMA_H

#define MAX_VARIABLES 15
#define MAX_RESTRICCIONES 15

// Estructura que representa el problema a resolver
typedef struct {
    int num_variables;
    int num_restricciones;
    double funcion_objetivo[MAX_VARIABLES];
    double restricciones[MAX_RESTRICCIONES][MAX_VARIABLES];
    double lado_derecho[MAX_RESTRICCIONES];
    int maximizar;
} Problema;

#endif