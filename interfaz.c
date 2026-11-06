#include "interfaz.h"
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "analisis.h"
#include "problema.h"
#include "salida.h"

// Los coeficientes deben ser razonables para que la aritmetica no se desborde
#define VALOR_MAXIMO 1e12

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

/*  Convierte el texto de un entry a numero real.
    Acepta punto o coma como separador decimal (en un sistema en espanol,
    atof() leeria "1.5" como 1). Devuelve FALSE si no es un numero valido. */
static gboolean leer_numero(GtkWidget *entry, double *valor) {
    gchar *texto = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
    gboolean ok = FALSE;

    g_strstrip(texto);
    g_strdelimit(texto, ",", '.');

    if (texto[0] != '\0') {
        gchar *fin = NULL;
        double v = g_ascii_strtod(texto, &fin);

        if (*fin == '\0' && isfinite(v) && fabs(v) <= VALOR_MAXIMO) {
            *valor = v;
            ok = TRUE;
        }
    }

    g_free(texto);
    return ok;
}

static void error_formato(GtkWindow *ventana, const gchar *formato, ...) {
    va_list args;
    va_start(args, formato);
    gchar *mensaje = g_strdup_vprintf(formato, args);
    va_end(args);

    mostrar_error(ventana, mensaje);
    g_free(mensaje);
}

/*  Lee y valida todos los datos de la interfaz y llena la estructura Problema.
    Si algo es invalido muestra el error y devuelve FALSE. */
static gboolean leer_problema(GtkWindow *ventana, AppData *app, Problema *p) {
    int V = s_data.num_vars;
    int R = s_data.num_rest;

    memset(p, 0, sizeof(Problema));
    p->num_variables = V;
    p->num_restricciones = R;
    g_strlcpy(p->nombre, app->nombre_problema, sizeof(p->nombre));

    gint objetivo = gtk_combo_box_get_active(GTK_COMBO_BOX(s_data.entry_opt));
    if (objetivo == 0) {
        p->maximizar = 1;
    } else if (objetivo == 1) {
        p->maximizar = 0;
    } else {
        mostrar_error(ventana, "Error: Seleccione Maximizar o Minimizar.");
        return FALSE;
    }

    // Nombres de las variables (hileras LaTeX; por defecto $x_1$, $x_2$, ...)
    for (int j = 0; j < V; j++) {
        gchar *nombre = g_strdup(gtk_entry_get_text(GTK_ENTRY(s_data.entries_nombres_vars[j])));
        g_strstrip(nombre);

        if (nombre[0] == '\0') {
            g_snprintf(p->nombres_variables[j], MAX_NOMBRE_VARIABLE, "X%d", j + 1);
        } else if (strlen(nombre) >= MAX_NOMBRE_VARIABLE) {
            g_free(nombre);
            error_formato(ventana, "Error: el nombre de la variable %d es demasiado largo (máximo %d caracteres).",
                          j + 1, MAX_NOMBRE_VARIABLE - 1);
            return FALSE;
        } else {
            g_strlcpy(p->nombres_variables[j], nombre, MAX_NOMBRE_VARIABLE);
        }
        g_free(nombre);
    }

    for (int j = 0; j < V; j++) {
        if (!leer_numero(s_data.entries_fo_coefs[j], &p->funcion_objetivo[j])) {
            error_formato(ventana, "Error: en la función objetivo, el coeficiente de la variable %d no es un número válido.", j + 1);
            return FALSE;
        }
    }

    for (int i = 0; i < R; i++) {
        for (int j = 0; j < V; j++) {
            if (!leer_numero(s_data.entries_rest_coefs[i][j], &p->restricciones[i][j])) {
                error_formato(ventana, "Error: en la restricción %d, el coeficiente de la variable %d no es un número válido.", i + 1, j + 1);
                return FALSE;
            }
        }

        if (!leer_numero(s_data.entries_rest_rhs[i], &p->lado_derecho[i])) {
            error_formato(ventana, "Error: en la restricción %d, el lado derecho no es un número válido.", i + 1);
            return FALSE;
        }

        if (p->lado_derecho[i] < 0) {
            error_formato(ventana, "Error: en la restricción %d, el lado derecho debe ser mayor o igual a cero.", i + 1);
            return FALSE;
        }
    }

    return TRUE;
}

void on_btn_resolver_clicked(GtkButton *button, gpointer user_data) {
    GtkWindow *ventana_actual = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    AppData *app = (AppData *)user_data;

    if (app == NULL || s_data.entry_opt == NULL) {
        return;
    }

    /*
    s_data.checkPresionado es un booleano: TRUE para mostrar todas las tablas
    intermedias del simplex, FALSE para mostrar solo la inicial y la final
    */
    if (s_data.check_ver_tablas) {
        s_data.checkPresionado = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(s_data.check_ver_tablas));
    }

    Problema problema;
    if (!leer_problema(ventana_actual, app, &problema)) {
        return;
    }

    Analisis analisis;
    if (analizar_problema(&problema, &analisis) != 0) {
        mostrar_error(ventana_actual, "Error: no hay memoria suficiente para resolver el problema.");
        return;
    }

    generar_salida(ventana_actual, &problema, &analisis, s_data.checkPresionado);

    liberar_analisis(&analisis);
}

static void actualizar_etiquetas_variables(GtkEditable *editable, gpointer data);

/* Refresca los nombres de variables en la funcion objetivo y restricciones. */
static void actualizar_etiquetas_variables(GtkEditable *editable, gpointer data) {
    (void)editable;
    (void)data;
    if (!s_data.entries_nombres_vars) return;

    for (int j = 0; j < s_data.num_vars; j++) {
        const gchar *nombre = gtk_entry_get_text(GTK_ENTRY(s_data.entries_nombres_vars[j]));
        gchar *texto = g_strdup_printf(" %s ", nombre[0] ? nombre : " ");
        if (s_data.labels_fo_vars && s_data.labels_fo_vars[j])
            gtk_label_set_text(GTK_LABEL(s_data.labels_fo_vars[j]), texto);
        for (int i = 0; i < s_data.num_rest; i++) {
            if (s_data.labels_rest_vars && s_data.labels_rest_vars[i] && s_data.labels_rest_vars[i][j])
                gtk_label_set_text(GTK_LABEL(s_data.labels_rest_vars[i][j]), texto);
        }
        g_free(texto);
    }
}

void construir_matriz_dinamica(GtkBuilder *builder, int num_vars, int num_rest) {
    GtkWidget *grid = GTK_WIDGET(gtk_builder_get_object(builder, "gridMatriz"));


    
    s_data.check_ver_tablas = GTK_WIDGET(gtk_builder_get_object(builder, "checkVerTablas"));

    gtk_container_foreach(GTK_CONTAINER(grid), (GtkCallback)destruir_hijo, NULL);

    if (s_data.entries_nombres_vars) g_free(s_data.entries_nombres_vars);
    if (s_data.entries_fo_coefs) g_free(s_data.entries_fo_coefs);
    if (s_data.labels_fo_vars) g_free(s_data.labels_fo_vars);
    if (s_data.labels_rest_vars) {
        for (int i = 0; i < s_data.num_rest; i++) {
            if (s_data.labels_rest_vars[i]) g_free(s_data.labels_rest_vars[i]);
        }
        g_free(s_data.labels_rest_vars);
    }
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
    s_data.labels_fo_vars = g_malloc(num_vars * sizeof(GtkWidget *));
    s_data.labels_rest_vars = g_malloc(num_rest * sizeof(GtkWidget **));
    for (int i = 0; i < num_rest; i++) {
        s_data.labels_rest_vars[i] = g_malloc(num_vars * sizeof(GtkWidget *));
    }

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
        g_snprintf(temp_label, sizeof(temp_label), "X%d", j + 1);
        gtk_entry_set_text(GTK_ENTRY(entry_nombre_var), temp_label);
        gtk_grid_attach(GTK_GRID(grid), entry_nombre_var, 1, fila, 1, 1);

        s_data.entries_nombres_vars[j] = entry_nombre_var;
        g_signal_connect(entry_nombre_var, "changed",
                         G_CALLBACK(actualizar_etiquetas_variables), NULL);
        fila++;
    }

    fila++; 

    GtkWidget *lbl_z_titulo = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(lbl_z_titulo), "<b>Función Objetivo:</b>");
    gtk_grid_attach(GTK_GRID(grid), lbl_z_titulo, 0, fila++, 1, 1);

    GtkWidget *box_fo = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    s_data.entry_opt = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(s_data.entry_opt), "Maximizar");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(s_data.entry_opt), "Minimizar");
    gtk_combo_box_set_active(GTK_COMBO_BOX(s_data.entry_opt), 0);
    gtk_widget_set_size_request(s_data.entry_opt, 120, -1);
    gtk_container_add(GTK_CONTAINER(box_fo), s_data.entry_opt);

    GtkWidget *label_z = gtk_label_new("   Z =");
    gtk_container_add(GTK_CONTAINER(box_fo), label_z);

    for (int j = 0; j < num_vars; j++) {
        s_data.entries_fo_coefs[j] = gtk_entry_new();
        gtk_entry_set_text(GTK_ENTRY(s_data.entries_fo_coefs[j]), "0");
        gtk_widget_set_size_request(s_data.entries_fo_coefs[j], 50, -1);
        gtk_container_add(GTK_CONTAINER(box_fo), s_data.entries_fo_coefs[j]);

        gchar temp_var[96];
        g_snprintf(temp_var, sizeof(temp_var), " %s ", gtk_entry_get_text(GTK_ENTRY(s_data.entries_nombres_vars[j])));
        GtkWidget *lbl_x = gtk_label_new(temp_var);
        s_data.labels_fo_vars[j] = lbl_x;
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

            gchar temp_var[96];
            g_snprintf(temp_var, sizeof(temp_var), " %s ", gtk_entry_get_text(GTK_ENTRY(s_data.entries_nombres_vars[j])));
            GtkWidget *lbl_x = gtk_label_new(temp_var);
            s_data.labels_rest_vars[i][j] = lbl_x;
            gtk_container_add(GTK_CONTAINER(box_rest), lbl_x);

            if (j < num_vars - 1) {
                GtkWidget *lbl_mas = gtk_label_new("+");
                gtk_container_add(GTK_CONTAINER(box_rest), lbl_mas);
            }
        }

        s_data.entries_rest_ops[i] = gtk_label_new("≤");
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