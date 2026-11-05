#include "interfaz.h"

// Callback auxiliar para manejar el cierre de las ventanas de forma segura
static void on_window_destroy(GtkWidget *widget, gpointer user_data) {
    g_print("🔔 [SIGNAL] Ventana cerrada. Saliendo de la aplicación...\n");
    gtk_main_quit();
}

int main(int argc, char *argv[]) {
    GtkBuilder *builder;
    GtkWidget *window1, *window2, *btn_continuar, *btn_resolver;

  
    gtk_init(&argc, &argv);

 
    builder = gtk_builder_new();
    
    if (gtk_builder_add_from_file(builder, "interface.ui", NULL) == 0) {
        g_printerr("ERROR CRÍTICO: No se pudo cargar el archivo 'interface.ui' ");
        return 1;
    }


    AppData app = {0};
    app.builder = builder;


    window1 = GTK_WIDGET(gtk_builder_get_object(builder, "window1"));
    window2 = GTK_WIDGET(gtk_builder_get_object(builder, "window2"));
   


    g_signal_connect(window1, "destroy", G_CALLBACK(on_window_destroy), NULL);
    g_signal_connect(window2, "destroy", G_CALLBACK(on_window_destroy), NULL);


    btn_continuar = GTK_WIDGET(gtk_builder_get_object(builder, "btnContinuar"));
    if (btn_continuar) {
        g_signal_connect(btn_continuar, "clicked", G_CALLBACK(on_btn_continuar_clicked), &app);
    } 

    btn_resolver = GTK_WIDGET(gtk_builder_get_object(builder, "btnResolver"));
    if (btn_resolver) {
        g_signal_connect(btn_resolver, "clicked", G_CALLBACK(on_btn_resolver_clicked), NULL);
    } 

 
    gtk_widget_show_all(window1);
    gtk_widget_hide(window2);


    gtk_main();

    g_object_unref(builder);
    return 0;
}

/*
gcc main_interfaz.c interfaz.c -o app $(pkg-config --cflags --libs gtk+-3.0)
./app
*/