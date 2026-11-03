#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "simplex.h"

/*  Selecciona la columna de la variable que debe entrar a la base
    según los coeficientes de la función objetivo.  */
int seleccionar_entrante(TablaSimplex *tabla, int maximizar) {

    int columna = -1;
    //Recorre las columnas de las variables de decisión y las variables de holgura.
    for (int j = 1; j < tabla->columna_recursos; j++) {
        double valor = tabla->valores[0][j];

        // Para maximización, busca coeficientes negativos
        if (maximizar) {
            if (valor < 0) {
                if (columna == -1 || valor < tabla->valores[0][columna]) {
                    columna = j;
                }
            }
        // Para minimización, busca coeficientes positivos
        } else {
            if (valor > 0) {

                if (columna == -1 || valor > tabla->valores[0][columna]) {
                    columna = j;
                }
            }
        }
    }
    return columna;
}


/*  Selecciona la fila de la variable que debe salir de la base
    mediante el cáluculo de razón mínima   */
int seleccionar_saliente(TablaSimplex *tabla, int columna_entrante) {

    int fila = -1;
    double razon_minima = 0;

    // Recorre las filas de las restricciones
    for (int i = 1; i < tabla->num_filas; i++) {
        // Obtiene el coeficiente de la variable entrante
        double coeficiente = tabla->valores[i][columna_entrante];

        if (coeficiente > 0) {
            // Obtiene el valor de los recursos disponibles de la restricción actual
            double recursos = tabla->valores[i][tabla->columna_recursos];
            // Calcula la razón entre los recursos y el coeficiente de la variable
            double razon = recursos / coeficiente;

            if (fila == -1 || razon < razon_minima) {
                // Guarda la nueva razón mínima encontrada.
                razon_minima = razon;
                fila = i;
            }
        }
    }
    // Devuelve el índice de la fila seleccionada
    return fila;
}


// Realiza el pivoteo para actualizar la tabla Simplex
void realizar_pivote(TablaSimplex *tabla, int fila_pivote, int columna_pivote) {

    // Obtiene el pivote
    double pivote = tabla->valores[fila_pivote][columna_pivote];

    /*  Divide todos los elementos de la fila pivote entre el pivote
        para convertir el pivote en 1 */
    for (int j = 0; j < tabla->num_columnas; j++) {
        tabla->valores[fila_pivote][j] /= pivote;
    }

    // Recorre todas las filas de la tabla para eliminar los demás valores
    for (int i = 0; i < tabla->num_filas; i++) {
        if (i != fila_pivote) {
            // Obtiene el coeficiente de la columna pivote en la fila actual
            double factor = tabla->valores[i][columna_pivote];
            for (int j = 0; j < tabla->num_columnas; j++) {
                tabla->valores[i][j] -= factor * tabla->valores[fila_pivote][j];
            }
        }
    }

    tabla->variables_basicas[fila_pivote - 1] = columna_pivote;
}

// Compara dos tablas Simplex para determinar si son iguales
int tablas_iguales(TablaSimplex *tabla1, TablaSimplex *tabla2) {
    for (int i = 0; i < tabla1->num_filas; i++) {
        for (int j = 0; j < tabla1->num_columnas; j++) {
            if (tabla1->valores[i][j] != tabla2->valores[i][j]) {
                return 0;
            }
        }
    }

    for (int i = 0; i < tabla1->num_restricciones; i++) {
        if (tabla1->variables_basicas[i] != tabla2->variables_basicas[i]) {
            return 0;
        }
    }

    return 1;
}


// Detecta si la tabla Simplex actual ya apareció anteriormente
int detectar_ciclo(TablaSimplex historial[], int cantidad_tablas, TablaSimplex *tabla_actual) {
    for (int i = 0; i < cantidad_tablas; i++) {
        if (tablas_iguales(&historial[i], tabla_actual)) {
            return 1;
        }
    }
    return 0;
}


/*  Ejecuta el método Simplex completo hasta encontrar una solución
o identificar alguna condición  */
ResultadoSimplex resolver_simplex(Problema *problema) {

    ResultadoSimplex resultado = {0};
    resultado.estado = SIMPLEX_ERROR;

    resultado.historial = calloc(MAX_ITERACIONES + 1, sizeof(TablaSimplex));

    if (resultado.historial == NULL) {
        return resultado;
    }

    // Declara la tabla que se utilizará para realizar las operaciones
    TablaSimplex tabla;

    // Construye la tabla inicial a partir de los datos del problema
    construir_tabla_inicial(problema, &tabla);

    resultado.historial[0] = tabla;
    resultado.cantidad_tablas = 1;

    // Inicia el ciclo principal del método Simplex
    // El algoritmo realizará como máximo MAX_ITERACIONES pivoteos
    for (int i = 0; i < MAX_ITERACIONES; i++) {

        int columna = seleccionar_entrante(&tabla, problema->maximizar);

        if (columna == -1) {
            resultado.estado = SIMPLEX_OPTIMO;
            break;
        }

        int fila = seleccionar_saliente(&tabla, columna);

        if (fila == -1) {
            resultado.estado = SIMPLEX_NO_ACOTADO;
            break;
        }

        realizar_pivote(&tabla, fila, columna);

        resultado.iteraciones++;

        if (detectar_ciclo(resultado.historial, resultado.cantidad_tablas, &tabla)) {
            resultado.estado = SIMPLEX_CICLAJE;
            resultado.historial[resultado.cantidad_tablas] = tabla;
            resultado.cantidad_tablas++;
            break;
        }

        resultado.historial[resultado.cantidad_tablas] = tabla;
        resultado.cantidad_tablas++;

        if (i == MAX_ITERACIONES - 1) {
            resultado.estado = SIMPLEX_LIMITE_ITERACIONES;
        }
    }
    return resultado;
}

void liberar_memoria(ResultadoSimplex *resultado) {
    free(resultado->historial);
    resultado->historial = NULL;
    resultado->cantidad_tablas = 0;
    resultado->iteraciones = 0;
}


/*  Extrae los valores de las variables de decisión,
    las variables de holgura y el valor de la función objetivo  */
void obtener_solucion(Problema *problema, TablaSimplex *tabla, double solucion[], double holguras[], double *valor_objetivo) {

    int n = problema->num_variables;
    int m = problema->num_restricciones;

    for (int j = 0; j < n; j++) {
        solucion[j] = 0.0;
    }

    for (int j = 0; j < m; j++) {
        holguras[j] = 0.0;
    }

    // Recorre todas las restricciones para identificar
    // las variables que forman parte de la base
    for (int i = 0; i < m; i++) {
        // Obtiene el índice de la columna
        int variable = tabla->variables_basicas[i];
        // Obtiene el valor de la variable básica
        double valor = tabla->valores[i + 1][tabla->columna_recursos];

        // Comprueba si la variable básica es de decisión
        if (variable >= 1 && variable <= n) {
            solucion[variable - 1] = valor;
        // Comprueba si la variable básica es una variable de holgura
        } else if (variable > n && variable <= n + m) {
            holguras[variable - n - 1] = valor;
        }
    }
    // Obtiene el valor final de la función objetivo
    *valor_objetivo = tabla->valores[0][tabla->columna_recursos];
}



int obtener_segunda_solucion(Problema *problema, TablaSimplex *tabla, TablaSimplex *segunda_tabla, double segunda_solucion[], double *segundo_valor_objetivo) {

    int n = problema->num_variables;
    int m = problema->num_restricciones;

    double solucion_original[MAX_VARIABLES];
    double holguras[MAX_RESTRICCIONES];
    double valor_original;

    obtener_solucion(problema, tabla, solucion_original, holguras, &valor_original);

    for (int j = 1; j < tabla->columna_recursos; j++) {
        int esta_en_base = 0;

        for (int i = 0; i < m; i++) {
            if (tabla->variables_basicas[i] == j) {
                esta_en_base = 1;
                break;
            }
        }

        if (esta_en_base) {
            continue;
        }

        if (tabla->valores[0][j] != 0) {
            continue;
        }

        int fila = seleccionar_saliente(tabla, j);

        if (fila == -1) {
            continue;
        }

        *segunda_tabla = *tabla;
        realizar_pivote(segunda_tabla, fila, j);
        double holguras_nuevas[MAX_RESTRICCIONES];

        obtener_solucion(problema, segunda_tabla, segunda_solucion, holguras_nuevas, segundo_valor_objetivo);

        int solucion_diferente = 0;

        for (int k = 0; k < n; k++) {
            if (solucion_original[k] != segunda_solucion[k]) {
                solucion_diferente = 1;
                break;
            }
        }

        if (solucion_diferente && valor_original == *segundo_valor_objetivo) {
            return 1;
        }
    }
    return 0;
}

void soluciones_adicionales(Problema *problema, double primera_solucion[], double segunda_solucion[], double soluciones[3][MAX_VARIABLES]) {

    int n = problema->num_variables;

    for (int i = 0; i < 3; i++) {
        double t = (i + 1) / 4.0;
        for (int j = 0; j < n; j++) {
            soluciones[i][j] = primera_solucion[j] + t * (segunda_solucion[j] - primera_solucion[j]);
        }
    }
}

