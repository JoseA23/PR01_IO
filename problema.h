#ifndef PROBLEMA_H
#define PROBLEMA_H

#define MAX_VARIABLES 15
#define MAX_RESTRICCIONES 15

// Tamanos maximos de las hileras de texto (incluyen el '\0')
#define MAX_NOMBRE_PROBLEMA 256
#define MAX_NOMBRE_VARIABLE 64

// Estructura que representa el problema a resolver
typedef struct {
    int num_variables;
    int num_restricciones;
    double funcion_objetivo[MAX_VARIABLES];
    double restricciones[MAX_RESTRICCIONES][MAX_VARIABLES];
    double lado_derecho[MAX_RESTRICCIONES];
    int maximizar;

    // Nombre del problema (texto libre, se usara en el documento LaTeX)
    char nombre[MAX_NOMBRE_PROBLEMA];
    // Nombres de las variables de decision (hileras LaTeX, ej. "$x_1$")
    char nombres_variables[MAX_VARIABLES][MAX_NOMBRE_VARIABLE];
} Problema;

#endif
