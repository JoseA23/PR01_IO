# Proyecto 1: Simplex
#
#   make            compila el programa (necesita GTK 3)
#   make run        compila y ejecuta
#   make test       pruebas del motor Simplex (no necesita GTK)
#   make test-gui   prueba de extremo a extremo de la interfaz (necesita xvfb-run)
#   make fuzz       compara el motor contra scipy con miles de problemas al azar (opcional)
#   make clean

CC         = gcc
CFLAGS     = -Wall -Wextra -Wno-unused-parameter -g -O2 -MMD -MP
LDLIBS     = -lm
GTK_CFLAGS = $(shell pkg-config --cflags gtk+-3.0)
GTK_LIBS   = $(shell pkg-config --libs gtk+-3.0)

PROGRAMA = Simplex

# Motor: no depende de GTK
MOTOR_OBJ = analisis.o simplex.o tabla.o
# Interfaz y salida: dependen de GTK
GUI_OBJ   = main.o interfaz.o salida.o

all: $(PROGRAMA)

$(PROGRAMA): $(GUI_OBJ) $(MOTOR_OBJ)
	$(CC) -o $@ $^ $(GTK_LIBS) $(LDLIBS)

$(GUI_OBJ): CFLAGS += $(GTK_CFLAGS)

run: $(PROGRAMA)
	./$(PROGRAMA)

# ---- Pruebas ----
SAN = -fsanitize=address,undefined

tests/test_motor: tests/test_motor.c analisis.c simplex.c tabla.c
	$(CC) -Wall -Wextra -g $(SAN) -I. -o $@ $^ $(LDLIBS)

test: tests/test_motor
	./tests/test_motor

tests/test_gui: tests/test_gui.c interfaz.c salida.c analisis.c simplex.c tabla.c
	$(CC) -Wall -Wextra -Wno-unused-parameter -g $(SAN) -I. $(GTK_CFLAGS) -o $@ $^ $(GTK_LIBS) $(LDLIBS)

test-gui: tests/test_gui
	ASAN_OPTIONS=detect_leaks=0 xvfb-run -a ./tests/test_gui

tests/harness: tests/harness.c simplex.c tabla.c
	$(CC) -Wall -Wextra -g $(SAN) -I. -o $@ $^ $(LDLIBS)

fuzz: tests/harness
	ASAN_OPTIONS=detect_leaks=0 python3 tests/fuzz.py 3000 1 ./tests/harness

clean:
	rm -f $(PROGRAMA) otro_simplex *.o *.d tests/test_motor tests/test_gui tests/harness

-include $(wildcard *.d)

.PHONY: all run test test-gui fuzz clean
