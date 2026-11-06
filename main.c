#include "interfaz.h"

// Callback auxiliar para manejar el cierre de las ventanas de forma segura
static void on_window_destroy(GtkWidget *widget, gpointer user_data) {
    gtk_main_quit();
}

/*  Carga interface.ui. Primero lo busca junto al ejecutable (asi funciona
    aunque se lance desde otra carpeta) y luego en la carpeta actual. */
static gboolean cargar_interfaz(GtkBuilder *builder) {
    GError *error = NULL;
    gboolean ok = FALSE;

    gchar *ruta_exe = g_file_read_link("/proc/self/exe", NULL);
    if (ruta_exe != NULL) {
        gchar *carpeta = g_path_get_dirname(ruta_exe);
        gchar *ruta_ui = g_build_filename(carpeta, "interface.ui", NULL);

        ok = (gtk_builder_add_from_file(builder, ruta_ui, &error) != 0);

        g_clear_error(&error);
        g_free(ruta_ui);
        g_free(carpeta);
        g_free(ruta_exe);
    }

    if (!ok) {
        ok = (gtk_builder_add_from_file(builder, "interface.ui", &error) != 0);
        g_clear_error(&error);
    }

    return ok;
}

int main(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget *window1, *window2, *btn_continuar, *btn_resolver;

    gtk_init(&argc, &argv);
    g_set_application_name("Simplex");

    builder = gtk_builder_new();

    if (!cargar_interfaz(builder)) {
        mostrar_error(NULL, "No se pudo cargar el archivo 'interface.ui'.\n"
                            "Debe estar en la misma carpeta que el programa.");
        return 1;
    }

    // AppData vive durante todo gtk_main(), asi que se puede compartir con los callbacks
    AppData app = {0};
    app.builder = builder;

    window1 = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
    window2 = GTK_WIDGET(gtk_builder_get_object(builder, "window2"));

    gtk_window_set_title(GTK_WINDOW(window1), "Simplex");
    gtk_window_set_title(GTK_WINDOW(window2), "Simplex");
    g_signal_connect(window1, "destroy", G_CALLBACK(on_window_destroy), NULL);
    g_signal_connect(window2, "destroy", G_CALLBACK(on_window_destroy), NULL);

    btn_continuar = GTK_WIDGET(gtk_builder_get_object(builder, "btnContinuar"));
    if (btn_continuar) {
        g_signal_connect(btn_continuar, "clicked", G_CALLBACK(on_btn_continuar_clicked), &app);
    }

    btn_resolver = GTK_WIDGET(gtk_builder_get_object(builder, "btnResolver"));
    if (btn_resolver) {
        g_signal_connect(btn_resolver, "clicked", G_CALLBACK(on_btn_resolver_clicked), &app);
    }

    gtk_widget_show_all(window1);
    gtk_widget_hide(window2);

    gtk_main();

    g_object_unref(builder);
    return 0;
}
