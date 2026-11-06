#ifndef INTERFAZ_H
#define INTERFAZ_H

#include <gtk/gtk.h>

typedef struct {
    int num_vars;
    int num_rest;
    gchar nombre_problema[256];
    GtkBuilder *builder;
} AppData;

typedef struct {
    int num_vars;
    int num_rest;
    GtkWidget *entry_opt;    // Selector: Maximizar / Minimizar           
    GtkWidget **entries_nombres_vars;   // Nombre de las variables
    GtkWidget **labels_fo_vars;       // Etiquetas dinamicas en funcion objetivo
    GtkWidget ***labels_rest_vars;    // Etiquetas dinamicas en restricciones
    GtkWidget **entries_fo_coefs;       //Coeficientes de la funciòn objetivo
    GtkWidget ***entries_rest_coefs;    //Coeficientes de las restricciones
    GtkWidget **entries_rest_ops;       // Etiquetas fijas de operador menor o igual
    GtkWidget **entries_rest_rhs;       // Lado derecho
    GtkWidget *check_ver_tablas;        // Chheckbox
    gboolean checkPresionado;           // Booleano para saber si mostrar todas las tablas o no
} SolverData;

extern SolverData s_data;

void mostrar_error(GtkWindow *parent, const gchar *mensaje);
void construir_matriz_dinamica(GtkBuilder *builder, int num_vars, int num_rest);
void on_btn_continuar_clicked(GtkButton *button, gpointer user_data);
void on_btn_resolver_clicked(GtkButton *button, gpointer user_data);

#endif