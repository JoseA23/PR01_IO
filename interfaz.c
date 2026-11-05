#include "interfaz.h"
#include <string.h>
#include <stdlib.h>

SolverData s_data = {0};

static void destruir_hijo(GtkWidget *widget, gpointer data) {
    gtk_widget_destroy(widget);
}

void mostrar_error(GtkWindow *parent, const gchar *mensaje) {
    GtkWidget *dialog = gtk_message_dialog_new(
        parent,
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_ERROR,
        GTK_BUTTONS_OK,
        "%s", mensaje
    );
    gtk_window_set_title(GTK_WINDOW(dialog), "Error de validación");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void on_btn_resolver_clicked(GtkButton *button, gpointer user_data) {
    GtkWindow *ventana_actual = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button)));

    if (s_data.check_ver_tablas) {
        s_data.checkPresionado = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(s_data.check_ver_tablas));
    }

    /*
    la variable s_data.checkPresionado es un booleano de True o False : True es para mostrar todas
    las tablas del simplex, y False es solo para mostrar la final
    */
    

    const gchar *modo = gtk_entry_get_text(GTK_ENTRY(s_data.entry_opt));
    
    if ((strcmp(modo, "Maximizar") != 0) && (strcmp(modo, "Minimizar") != 0)) {
        mostrar_error(ventana_actual, "Error: Debe ingresar Maximizar ó Minimizar");
        return;
    }

    /*
    Usar la variable "modo", para meterlo a la funciòn del sìmplex
    */

    gboolean operador_invalido = FALSE;
    for (int i = 0; i < s_data.num_rest; i++) {
        const gchar *op_str = gtk_entry_get_text(GTK_ENTRY(s_data.entries_rest_ops[i]));
        if (strcmp(op_str, "<=") != 0) {
            operador_invalido = TRUE;
            break;
        }
    }

    if (operador_invalido) {
        mostrar_error(ventana_actual, "Error: Por ahora solo se permite el operador <= en las restricciones.");
        return;
    }

    int V = s_data.num_vars;
    int R = s_data.num_rest;
    int total_filas = R + 1;
    int total_cols = V + R + 2;

    double **tabla = g_malloc(total_filas * sizeof(double *));
    for (int i = 0; i < total_filas; i++) {
        tabla[i] = g_malloc0(total_cols * sizeof(double));
    }

    tabla[0][0] = 1.0; 
    for (int j = 0; j < V; j++) {
        const gchar *val_str = gtk_entry_get_text(GTK_ENTRY(s_data.entries_fo_coefs[j]));
        tabla[0][1 + j] = atof(val_str);
    }

    for (int i = 0; i < R; i++) {
        int fila_tabla = i + 1;
        tabla[fila_tabla][0] = 0.0; 

        for (int j = 0; j < V; j++) {
            const gchar *coef_str = gtk_entry_get_text(GTK_ENTRY(s_data.entries_rest_coefs[i][j]));
            tabla[fila_tabla][1 + j] = atof(coef_str);
        }

        tabla[fila_tabla][1 + V + i] = 1.0;

        const gchar *rhs_str = gtk_entry_get_text(GTK_ENTRY(s_data.entries_rest_rhs[i]));
        tabla[fila_tabla][total_cols - 1] = atof(rhs_str);
    }

    /*
    La variable tabla tiene la taba de sìmplex en forma de matriz, esos elementos se deben transformar
    a typedef struct TablaSimplex y la de Problema:
    en dado caso, la variable s_data que es un typedef struct (veanlo en interfaz.h) tiene guardados todos los datos
    del problema
    */

    /*
    Aquì se transforma la tabla de interfaz gràfica a TablaSimplex y Problema y se ejectuta el algoritmo, para luego 
    pasarlo a latex
    */

    for (int i = 0; i < total_filas; i++) {
        g_free(tabla[i]);
    }
    g_free(tabla);
}

void construir_matriz_dinamica(GtkBuilder *builder, int num_vars, int num_rest) {
    GtkWidget *grid = GTK_WIDGET(gtk_builder_get_object(builder, "gridMatriz"));


    
    s_data.check_ver_tablas = GTK_WIDGET(gtk_builder_get_object(builder, "checkVerTablas"));

    gtk_container_foreach(GTK_CONTAINER(grid), (GtkCallback)destruir_hijo, NULL);

    if (s_data.entries_nombres_vars) g_free(s_data.entries_nombres_vars);
    if (s_data.entries_fo_coefs) g_free(s_data.entries_fo_coefs);
    if (s_data.entries_rest_coefs) {
        for (int i = 0; i < s_data.num_rest; i++) {
            if (s_data.entries_rest_coefs[i]) g_free(s_data.entries_rest_coefs[i]);
        }
        g_free(s_data.entries_rest_coefs);
    }
    if (s_data.entries_rest_ops) g_free(s_data.entries_rest_ops);
    if (s_data.entries_rest_rhs) g_free(s_data.entries_rest_rhs);

    s_data.num_vars = num_vars;
    s_data.num_rest = num_rest;
    s_data.entries_nombres_vars = g_malloc(num_vars * sizeof(GtkWidget *));
    s_data.entries_fo_coefs = g_malloc(num_vars * sizeof(GtkWidget *));
    
    s_data.entries_rest_coefs = g_malloc(num_rest * sizeof(GtkWidget **));
    for (int i = 0; i < num_rest; i++) {
        s_data.entries_rest_coefs[i] = g_malloc(num_vars * sizeof(GtkWidget *));
    }
    s_data.entries_rest_ops = g_malloc(num_rest * sizeof(GtkWidget *));
    s_data.entries_rest_rhs = g_malloc(num_rest * sizeof(GtkWidget *));

    int fila = 0;

    GtkWidget *lbl_titulo_vars = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(lbl_titulo_vars), "<b>Nombrar Variables de Decisión:</b>");
    gtk_grid_attach(GTK_GRID(grid), lbl_titulo_vars, 0, fila++, 2, 1);

    for (int j = 0; j < num_vars; j++) {
        gchar temp_label[32];
        g_snprintf(temp_label, sizeof(temp_label), "x%d:", j + 1);
        GtkWidget *lbl_var = gtk_label_new(temp_label);
        gtk_widget_set_halign(lbl_var, GTK_ALIGN_START);
        gtk_grid_attach(GTK_GRID(grid), lbl_var, 0, fila, 1, 1);

        GtkWidget *entry_nombre_var = gtk_entry_new();
        g_snprintf(temp_label, sizeof(temp_label), "x%d", j + 1);
        gtk_entry_set_text(GTK_ENTRY(entry_nombre_var), temp_label);
        gtk_grid_attach(GTK_GRID(grid), entry_nombre_var, 1, fila, 1, 1);

        s_data.entries_nombres_vars[j] = entry_nombre_var;
        fila++;
    }

    fila++; 

    GtkWidget *lbl_z_titulo = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(lbl_z_titulo), "<b>Función Objetivo:</b>");
    gtk_grid_attach(GTK_GRID(grid), lbl_z_titulo, 0, fila++, 1, 1);

    GtkWidget *box_fo = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    s_data.entry_opt = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(s_data.entry_opt), "Maximizar");
    gtk_widget_set_size_request(s_data.entry_opt, 100, -1);
    gtk_container_add(GTK_CONTAINER(box_fo), s_data.entry_opt);

    GtkWidget *label_z = gtk_label_new("   Z =");
    gtk_container_add(GTK_CONTAINER(box_fo), label_z);

    for (int j = 0; j < num_vars; j++) {
        s_data.entries_fo_coefs[j] = gtk_entry_new();
        gtk_entry_set_text(GTK_ENTRY(s_data.entries_fo_coefs[j]), "0");
        gtk_widget_set_size_request(s_data.entries_fo_coefs[j], 50, -1);
        gtk_container_add(GTK_CONTAINER(box_fo), s_data.entries_fo_coefs[j]);

        gchar temp_var[32];
        g_snprintf(temp_var, sizeof(temp_var), " x%d ", j + 1);
        GtkWidget *lbl_x = gtk_label_new(temp_var);
        gtk_container_add(GTK_CONTAINER(box_fo), lbl_x);

        if (j < num_vars - 1) {
            GtkWidget *lbl_mas = gtk_label_new("+");
            gtk_container_add(GTK_CONTAINER(box_fo), lbl_mas);
        }
    }
    gtk_grid_attach(GTK_GRID(grid), box_fo, 0, fila++, 1, 1);

    GtkWidget *label_sujeto = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label_sujeto), "<b>Sujeto a:</b>");
    gtk_widget_set_halign(label_sujeto, GTK_ALIGN_START);
    gtk_grid_attach(GTK_GRID(grid), label_sujeto, 0, fila++, 1, 1);

    for (int i = 0; i < num_rest; i++) {
        GtkWidget *box_rest = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

        for (int j = 0; j < num_vars; j++) {
            s_data.entries_rest_coefs[i][j] = gtk_entry_new();
            gtk_entry_set_text(GTK_ENTRY(s_data.entries_rest_coefs[i][j]), "0");
            gtk_widget_set_size_request(s_data.entries_rest_coefs[i][j], 50, -1);
            gtk_container_add(GTK_CONTAINER(box_rest), s_data.entries_rest_coefs[i][j]);

            gchar temp_var[32];
            g_snprintf(temp_var, sizeof(temp_var), " x%d ", j + 1);
            GtkWidget *lbl_x = gtk_label_new(temp_var);
            gtk_container_add(GTK_CONTAINER(box_rest), lbl_x);

            if (j < num_vars - 1) {
                GtkWidget *lbl_mas = gtk_label_new("+");
                gtk_container_add(GTK_CONTAINER(box_rest), lbl_mas);
            }
        }

        s_data.entries_rest_ops[i] = gtk_entry_new();
        gtk_entry_set_text(GTK_ENTRY(s_data.entries_rest_ops[i]), "<=");
        gtk_widget_set_size_request(s_data.entries_rest_ops[i], 45, -1);
        gtk_container_add(GTK_CONTAINER(box_rest), s_data.entries_rest_ops[i]);

        s_data.entries_rest_rhs[i] = gtk_entry_new();
        gtk_entry_set_text(GTK_ENTRY(s_data.entries_rest_rhs[i]), "0");
        gtk_widget_set_size_request(s_data.entries_rest_rhs[i], 50, -1);
        gtk_container_add(GTK_CONTAINER(box_rest), s_data.entries_rest_rhs[i]);

        gtk_grid_attach(GTK_GRID(grid), box_rest, 0, fila++, 1, 1);
    }
}

void on_btn_continuar_clicked(GtkButton *button, gpointer user_data) {
    AppData *app = (AppData *)user_data;

    GtkWidget *spin_vars = GTK_WIDGET(gtk_builder_get_object(app->builder, "spinVariables"));
    GtkWidget *spin_rest = GTK_WIDGET(gtk_builder_get_object(app->builder, "spinRestricciones"));
    GtkWidget *entry_nombre = GTK_WIDGET(gtk_builder_get_object(app->builder, "NombreProblema"));
    GtkWidget *window1 = GTK_WIDGET(gtk_builder_get_object(app->builder, "window1"));
    GtkWidget *window2 = GTK_WIDGET(gtk_builder_get_object(app->builder, "window2"));

    app->num_vars = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_vars));
    app->num_rest = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spin_rest));
    
    const gchar *nombre = gtk_entry_get_text(GTK_ENTRY(entry_nombre));
    g_strlcpy(app->nombre_problema, (strlen(nombre) > 0) ? nombre : "Sin nombre", sizeof(app->nombre_problema));

    if ((app->num_vars > 15) || (app->num_vars < 2)) {
        mostrar_error(GTK_WINDOW(window1), "Error: la cantidad de variables debe estar entre 2 y 15");
        return;
    }

    if ((app->num_rest < 2) || (app->num_rest > 15)) {
        mostrar_error(GTK_WINDOW(window1), "Error: la cantidad de restricciones debe estar entre 2 y 15");
        return;
    }

    gtk_widget_hide(window1);
    construir_matriz_dinamica(app->builder, app->num_vars, app->num_rest);
    gtk_widget_show_all(window2);
}