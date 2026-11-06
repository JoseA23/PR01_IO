#ifndef SALIDA_H
#define SALIDA_H

#include <gtk/gtk.h>

#include "analisis.h"
#include "problema.h"

/* Genera el informe LaTeX, lo compila a PDF con pdflatex y lo abre con Evince.
   mostrar_intermedias controla si se documentan todas las tablas del historial. */
void generar_salida(GtkWindow *parent, const Problema *problema,
                    const Analisis *analisis, gboolean mostrar_intermedias);

#endif
