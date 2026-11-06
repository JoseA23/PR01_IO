#include "salida.h"
#include "interfaz.h"

#include <glib/gstdio.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEX_CAP 262144

static void tex_escapar(GString *s, const char *texto) {
    if (!texto) return;
    for (const char *p = texto; *p; p++) {
        switch (*p) {
            case '#': g_string_append(s, "\\#"); break;
            case '$': g_string_append(s, "\\$"); break;
            case '%': g_string_append(s, "\\%"); break;
            case '&': g_string_append(s, "\\&"); break;
            case '_': g_string_append(s, "\\_"); break;
            case '{': g_string_append(s, "\\{"); break;
            case '}': g_string_append(s, "\\}"); break;
            case '~': g_string_append(s, "\\textasciitilde{}"); break;
            case '^': g_string_append(s, "\\textasciicircum{}"); break;
            case '\\': g_string_append(s, "\\textbackslash{}"); break;
            default: g_string_append_c(s, *p); break;
        }
    }
}

static const char *nombre_var(const Problema *p, int j) {
    return p->nombres_variables[j][0] ? p->nombres_variables[j] : "X";
}

static void agregar_encabezado(GString *s, const Problema *p) {
    g_string_append(s, "\\documentclass[11pt,oneside]{report}\n"
        "\\usepackage[utf8]{inputenc}\n\\usepackage[T1]{fontenc}\n"
        "\\usepackage[spanish,es-nodecimaldot]{babel}\n"
        "\\usepackage{lmodern,microtype}\n\\usepackage[a4paper,margin=2.3cm]{geometry}\n"
        "\\usepackage[table]{xcolor}\n\\usepackage{colortbl,booktabs,array,longtable,float}\n"
        "\\usepackage{amsmath,amssymb}\n\\usepackage{graphicx}\n"
        "\\usepackage{fancyhdr}\n\\usepackage{hyperref}\n"
        "\\definecolor{azul}{HTML}{17365D}\n"
        "\\definecolor{turquesa}{HTML}{168C91}\n"
        "\\definecolor{azulclaro}{HTML}{EAF2F8}\n"
        "\\definecolor{dorado}{HTML}{D6A84B}\n"
        "\\definecolor{verde}{HTML}{DDF2E5}\n"
        "\\definecolor{rojo}{HTML}{FCE4E4}\n"
        "\\hypersetup{colorlinks=true,linkcolor=black,urlcolor=black,citecolor=black}\n"
        "\\pagestyle{fancy}\\fancyhf{}\\lhead{Proyecto Simplex}"
        "\\rhead{Investigación de Operaciones}\\cfoot{\\thepage}\n"
        "\\setlength{\\parskip}{5pt}\\setlength{\\parindent}{0pt}\n"
        "\\begin{document}\n");
    (void)p;
}

static void agregar_portada(GString *s, const Problema *p) {
    g_string_append(s, "\\begin{titlepage}\n\\centering\n\\vspace*{1.2cm}\n"
        "{\\Large Instituto Tecnológico de Costa Rica}\\\\[0.3cm]\n"
        "{\\large Escuela de Ingeniería en Computación}\\\\[1.7cm]\n"
        "{\\rule{0.85\\textwidth}{1.2pt}}\\\\[0.8cm]\n"
        "{\\Huge\\bfseries Proyecto Programado 1}\\\\[0.35cm]\n"
        "{\\LARGE\\bfseries Método Simplex}\\\\[0.8cm]\n"
        "{\\large Informe de ejecución y resultados}\\\\[0.5cm]\n"
        "{\\rule{0.55\\textwidth}{1pt}}\\\\[1.2cm]\n"
        "\\begin{minipage}{0.82\\textwidth}\\centering\\large\n"
        "\\textbf{Integrantes}\\\\[0.35cm]\n"
        "Jose Andrés Benavides Díaz\\\\\n"
        "Axel Fabián Chavarría Gutiérrez\\\\\n"
        "Christopher Jiménez Gutiérrez\\\\[0.8cm]\n"
        "\\textbf{Curso:} IC6400 -- Investigación de Operaciones\\\\\n"
        "\\textbf{Periodo:} II Semestre 2026\\\\\n"
        "\\textbf{Fecha:} Noviembre 2026\n"
        "\\end{minipage}\\vfill\n{\\large San José, Costa Rica}\\\\\n"
        "\\end{titlepage}\n\\tableofcontents\\clearpage\n");
    g_string_append(s, "\\chapter{Introducción}\n"
        "La programación lineal es una técnica matemática que permite resolver problemas de maximización o minimización de una función objetivo lineal, sujeta a un conjunto de restricciones. El método símplex resuelve estos problemas mediante operaciones algebraicas sobre una tabla: transforma las restricciones en ecuaciones utilizando variables de holgura y recorre soluciones básicas factibles hasta encontrar una solución óptima o identificar una condición especial.\n\n"
        "\\section{Antecedentes}\n"
        "La programación lineal surge como una herramienta matemática para modelar y resolver problemas de optimización, permitiendo maximizar o minimizar una función objetivo lineal sujeta a restricciones expresadas mediante desigualdades o igualdades. Su objetivo es proporcionar un marco analítico estructurado que pueda ser resuelto de manera eficiente mediante métodos algorítmicos.\n\n"
        "En 1947, el matemático George Dantzig, durante su labor en la Fuerza Aérea de los Estados Unidos, ideó el método Simplex. Dantzig desarrolló este algoritmo para dar solución a problemas de planificación y logística militar, estableciendo un procedimiento que permite recorrer de forma eficiente los vértices (puntos de un área) de la región factible (región que cumple todas las restricciones) sin necesidad de evaluarlos todos por fuerza bruta, optimizando así la búsqueda de la solución óptima [1].\n");

    g_string_append(s, "\\chapter{Diseño de la arquitectura y la interfaz gráfica}\n"
        "La aplicación está diseñada para que el usuario, mediante una interfaz gráfica, pueda seleccionar la cantidad de variables, restricciones y el nombre del problema, para seguidamente ingresar los coeficientes de la matriz símplex y los nombres de las variables.\n\n"
        "\\section{Entorno tecnológico}\n"
        "El desarrollo del proyecto se llevó a cabo bajo los siguientes entornos tecnológicos:\n"
        "\\begin{itemize}\n"
        "\\item \\textbf{Lenguaje C:} Lenguaje utilizado para implementar el programa y gestionar la memoria.\n"
        "\\item \\textbf{Sistema operativo Linux:} El proyecto está preparado para compilarse y ejecutarse en este sistema operativo, junto con las herramientas requeridas para su funcionamiento.\n"
        "\\item \\textbf{GTK y Glade:} Se utilizaron para desarrollar la interfaz gráfica y separar la presentación visual de la lógica del programa.\n"
        "\\end{itemize}\n"
        "\\section{Diseño de la interfaz}\n"
        "La interfaz gráfica permite ingresar los datos del problema de forma organizada, facilitando la configuración de las variables, la función objetivo y las restricciones.\n\n"
        "\\subsection{Generación dinámica de widgets}\n"
        "La primera ventana de la interfaz gráfica contiene los siguientes elementos estáticos:\n"
        "\\begin{itemize}\n"
        "\\item Un espacio para ingresar el nombre del problema.\n"
        "\\item Un botón para continuar a la siguiente ventana, donde se generan los widgets de manera dinámica.\n"
        "\\item Controles de incremento y decremento para definir la cantidad de variables y restricciones. También permiten escribir directamente un valor numérico.\n"
        "\\end{itemize}\n"
        "La segunda ventana contiene un GtkGrid cuyas celdas se generan dinámicamente a partir de los datos ingresados por el usuario. El programa construye los elementos de la siguiente manera:\n"
        "\\begin{itemize}\n"
        "\\item Una fila por cada variable $X_i$: incluye la etiqueta de la variable y un campo para asignarle un nombre (por defecto, $X_i$), además de las columnas correspondientes a las variables.\n"
        "\\item Una fila para la función objetivo con los siguientes elementos:\n"
        "  \\begin{itemize}\n"
        "  \\item Un selector del tipo de optimización, con las opciones \"Maximizar\" y \"Minimizar\".\n"
        "  \\item La etiqueta \"Z=\" para identificar la función objetivo.\n"
        "  \\item Campos numéricos para ingresar el coeficiente de cada variable.\n"
        "  \\end{itemize}\n"
        "\\item Una fila con la etiqueta \"Sujeto a:\" para identificar la sección de restricciones.\n"
        "\\item Una fila para cada restricción, con un campo numérico para el coeficiente de cada variable, la relación fija \\(\\leq\\) y un campo para ingresar el lado derecho de la inecuación.\n"
        "\\item Una fila con el botón \"Resolver\" para ejecutar el algoritmo símplex.\n"
        "\\item Una casilla de verificación para elegir si se muestran todas las tablas del procedimiento o únicamente la tabla final.\n"
        "\\end{itemize}\n");
    g_string_append(s, "\\chapter{Problema original}\n\\textbf{Nombre:} ");
    tex_escapar(s, p->nombre[0] ? p->nombre : "Sin nombre");
    g_string_append(s, "\\\\\n\\textbf{Objetivo:} ");
    g_string_append(s, p->maximizar ? "Maximizar" : "Minimizar");
    g_string_append(s, "\\[ Z = ");
    for (int j = 0; j < p->num_variables; j++) {
        if (j) g_string_append(s, " + ");
        g_string_append_printf(s, "%.6g\\,\\text{", p->funcion_objetivo[j]);
        tex_escapar(s, nombre_var(p,j));
        g_string_append_c(s, '}');
    }
    g_string_append(s, " \\]\n\\textbf{Sujeto a:}\n\\begin{align*}\n");
    for (int i = 0; i < p->num_restricciones; i++) {
        for (int j = 0; j < p->num_variables; j++) {
            if (j) g_string_append(s, " + ");
            g_string_append_printf(s, "%.6g\\,\\text{", p->restricciones[i][j]);
            tex_escapar(s, nombre_var(p,j));
            g_string_append_c(s, '}');
        }
        g_string_append_printf(s, " &\\leq %.6g \\\\\n", p->lado_derecho[i]);
    }
    g_string_append(s, "\\text{Todas las variables son no negativas.}\\end{align*}\n");
}

static void agregar_tabla(GString *s, const Problema *p, const TablaSimplex *t,
                          const char *titulo, int resaltar, int col_pivote, int fila_pivote) {
    int n=p->num_variables, m=p->num_restricciones;
    g_string_append_printf(s, "\\subsection{%s}\n\\begin{table}[H]\\centering\\small\n", titulo);
    g_string_append(s, "\\rowcolors{2}{azulclaro}{white}\n\\begin{tabular}{c");
    for(int j=0;j<n+m+1;j++) g_string_append_c(s,'r');
    g_string_append(s, "}\\toprule\nBase");
    for(int j=0;j<n;j++){ g_string_append(s," & "); tex_escapar(s,nombre_var(p,j)); }
    for(int j=0;j<m;j++) g_string_append_printf(s," & $s_{%d}$",j+1);
    g_string_append(s," & RHS \\\\\n\\midrule\n");
    for(int i=0;i<t->num_filas;i++){
        if(i==0) g_string_append(s,"$Z$");
        else if(t->variables_basicas[i-1]>=1 && t->variables_basicas[i-1]<=n){g_string_append(s,"$\\text{");tex_escapar(s,nombre_var(p,t->variables_basicas[i-1]-1));g_string_append(s,"}$");}
        else g_string_append_printf(s,"$s_{%d}$",t->variables_basicas[i-1]-n);
        for(int j=1;j<t->num_columnas;j++){
            g_string_append(s," & ");
            if(resaltar && j==col_pivote) g_string_append(s,"\\cellcolor{verde}");
            if(resaltar && i==fila_pivote && j==col_pivote) g_string_append(s,"\\cellcolor{dorado!55}\\bfseries ");
            g_string_append_printf(s,"%.5g",t->valores[i][j]);
        }
        g_string_append(s," \\\\\n");
    }
    g_string_append(s,"\\bottomrule\\end{tabular}\\end{table}\n");
}

static void agregar_historial(GString *s, const Problema *p, const Analisis *a, gboolean todas) {
    const ResultadoSimplex *r=&a->simplex;
    g_string_append(s,"\\chapter{Desarrollo del algoritmo}\n");
    if(!r->historial || r->cantidad_tablas==0){g_string_append(s,"No se generó un historial de tablas.\n");return;}
    agregar_tabla(s,p,&r->historial[0],"Tabla inicial",0,-1,-1);
    int inicio=1, fin=r->cantidad_tablas-1;
    if(todas){
        for(int k=1;k<r->cantidad_tablas-1;k++){
            int fila=-1,col=-1;
            const TablaSimplex *prev=&r->historial[k-1], *cur=&r->historial[k];
            for(int i=0;i<p->num_restricciones;i++) if(prev->variables_basicas[i]!=cur->variables_basicas[i]){fila=i+1;col=cur->variables_basicas[i];break;}
            gchar *tt=g_strdup_printf("Tabla después de la iteración %d",k);
            agregar_tabla(s,p,cur,tt,1,col,fila); g_free(tt);
            if(fila>0 && col>0){
                double den=prev->valores[fila][col];
                g_string_append_printf(s,"\\textbf{Pivote:} %.6g.\\quad \\textbf{Razón de la fila pivote:} $%.6g/%.6g = %.6g$.\\par\n",den,prev->valores[fila][prev->columna_recursos],den,prev->valores[fila][prev->columna_recursos]/den);
            }
        }
    }
    if(fin>=inicio) {
        int fila=-1,col=-1;
        const TablaSimplex *prev=&r->historial[fin-1], *cur=&r->historial[fin];
        for(int i=0;i<p->num_restricciones;i++) {
            if(prev->variables_basicas[i]!=cur->variables_basicas[i]) {
                fila=i+1;
                col=cur->variables_basicas[i];
                break;
            }
        }
        agregar_tabla(s,p,cur,"Tabla final",fila>0,col,fila);
        if(fila>0 && col>0) {
            double pivote=prev->valores[fila][col];
            double rhs=prev->valores[fila][prev->columna_recursos];
            g_string_append_printf(s,
                "\\textbf{Pivote de la última iteración:} %.6g.\\quad "
                "\\textbf{Razón de la fila pivote:} $%.6g/%.6g = %.6g$.\\par\n",
                pivote,rhs,pivote,rhs/pivote);
        }
    }
    if(a->multiples) agregar_tabla(s,p,&a->segunda_tabla,"Segunda tabla final (solución alternativa)",0,-1,-1);
}

static void agregar_resultado(GString *s,const Problema *p,const Analisis *a){
    g_string_append(s,"\\chapter{Solución y análisis de resultados}\n");
    switch(a->simplex.estado){
        case SIMPLEX_OPTIMO:
            g_string_append_printf(s,"\\begin{center}\\fbox{\\parbox{0.88\\textwidth}{\\centering\\Large\\textbf{Solución óptima encontrada}}}\\end{center}\n\\begin{align*}\\text{Valor óptimo: } Z^* &= %.8g\\\\\n",a->valor_objetivo);
            for(int j=0;j<p->num_variables;j++){g_string_append(s,"\\text{");tex_escapar(s,nombre_var(p,j));g_string_append_printf(s,"} &= %.8g\\\\\n",a->solucion[j]);}
            g_string_append(s,"\\end{align*}\n\\section{Variables de holgura}\n\\begin{itemize}\n");
            for(int i=0;i<p->num_restricciones;i++)g_string_append_printf(s,"\\item $s_{%d}=%.8g$\n",i+1,a->holguras[i]);
            g_string_append(s,"\\end{itemize}\n");
            if(a->multiples){
                g_string_append_printf(s,"\\section{Soluciones múltiples}\nSe identificó otra solución óptima con el mismo valor objetivo $Z=%.8g$.\\begin{align*}\n",a->segundo_valor_objetivo);
                for(int j=0;j<p->num_variables;j++){g_string_append(s,"\\text{");tex_escapar(s,nombre_var(p,j));g_string_append_printf(s,"} &= %.8g\\\\\n",a->segunda_solucion[j]);}
                g_string_append(s,"\\end{align*}Las soluciones sobre el segmento entre ambas soluciones también son óptimas.\n\\begin{itemize}\n");
                for(int k=0;k<3;k++){g_string_append_printf(s,"\\item Solución alternativa %d: ",k+1);for(int j=0;j<p->num_variables;j++){if(j)g_string_append(s,", ");tex_escapar(s,nombre_var(p,j));g_string_append_printf(s,"=%.5g",a->adicionales[k][j]);}g_string_append_c(s,'\n');}
                g_string_append(s,"\\end{itemize}\n");
            }
            if(a->degenerado)g_string_append_printf(s,"\\section{Caso degenerado}\nSe detectó una variable básica con valor cero en la tabla %d del historial. Los empates de la prueba de razón mínima se resuelven conservando la fila de menor índice.\n",a->tabla_degenerada);
            break;
        case SIMPLEX_NO_ACOTADO:g_string_append(s,"\\begin{center}\\fbox{\\parbox{0.88\\textwidth}{\\centering\\Large\\textbf{Problema no acotado}}}\\end{center}La función objetivo puede mejorar indefinidamente dentro de la región factible.\n");break;
        case SIMPLEX_CICLAJE:g_string_append(s,"Se detectó ciclaje: una tabla del procedimiento volvió a aparecer.\n");break;
        case SIMPLEX_LIMITE_ITERACIONES:g_string_append_printf(s,"Se alcanzó el límite de %d iteraciones.\n",MAX_ITERACIONES);break;
        default:g_string_append(s,"No fue posible determinar una solución debido a un error del algoritmo.\n");break;
    }
    g_string_append_printf(s,"\\section{Resumen de ejecución}\nSe generaron %d tablas y se realizaron %d iteraciones.\n",a->simplex.cantidad_tablas,a->simplex.iteraciones);
}

static gboolean escribir_archivo(const char *ruta,const char *contenido){
    GError *error=NULL; gboolean ok=g_file_set_contents(ruta,contenido,-1,&error);
    if(!ok){g_warning("No se pudo escribir %s: %s",ruta,error?error->message:"error desconocido");g_clear_error(&error);} return ok;
}

void generar_salida(GtkWindow *parent, const Problema *problema,
                    const Analisis *analisis, gboolean mostrar_intermedias) {
    gchar *directorio=g_get_current_dir();
    gchar *tex=g_build_filename(directorio,"Informe_Simplex.tex",NULL);
    gchar *pdf=g_build_filename(directorio,"Informe_Simplex.pdf",NULL);
    GString *doc=g_string_sized_new(TEX_CAP);
    agregar_encabezado(doc,problema); agregar_portada(doc,problema);
    agregar_historial(doc,problema,analisis,mostrar_intermedias);
    agregar_resultado(doc,problema,analisis);
    g_string_append(doc,"\\chapter{Referencias}\n"
        "\\renewcommand{\\labelenumi}{[\\theenumi]}\n"
        "\\begin{enumerate}\n"
        "\\item G. B. Dantzig, \\textit{Linear Programming and Extensions}. Princeton University Press, 1963.\n"
        "\\item F. S. Hillier y G. J. Lieberman, \\textit{Introduction to Operations Research}. McGraw-Hill.\n"
        "\\end{enumerate}\n\\end{document}\n");
    gboolean escrito=escribir_archivo(tex,doc->str);
    g_string_free(doc,TRUE);
    if(!escrito){mostrar_error(parent,"No se pudo generar Informe_Simplex.tex. Revise los permisos de la carpeta.");goto fin;}

    gchar *comando=g_strdup_printf("pdflatex -interaction=nonstopmode -halt-on-error -output-directory=\"%s\" \"%s\"",directorio,tex);
    gchar *salida=NULL,*error=NULL; gint estado=0;
    gboolean ejecutado=g_spawn_command_line_sync(comando,&salida,&error,&estado,NULL);
    if (ejecutado && estado == 0) {
        g_clear_pointer(&salida, g_free);
        g_clear_pointer(&error, g_free);
        estado = 0;
        ejecutado=g_spawn_command_line_sync(comando,&salida,&error,&estado,NULL);
    }
    g_free(comando);
    if(!ejecutado || estado!=0){
        gchar *msg=g_strdup_printf("No se pudo compilar el PDF con pdflatex.\nInstale LaTeX (texlive-latex-extra) y revise Informe_Simplex.log.\n%s",error?error:(salida?salida:""));
        mostrar_error(parent,msg);g_free(msg);
    } else {
        gchar *abrir=g_strdup_printf("evince \"%s\"",pdf);
        GError *err=NULL;
        if(!g_spawn_command_line_async(abrir,&err)){
            gchar *msg=g_strdup_printf("El PDF se generó correctamente, pero no se pudo abrir con Evince: %s",err?err->message:"error desconocido");
            mostrar_error(parent,msg);g_free(msg);g_clear_error(&err);
        }
        g_free(abrir);
    }
    g_free(salida);g_free(error);
fin:
    g_free(tex);g_free(pdf);g_free(directorio);
}