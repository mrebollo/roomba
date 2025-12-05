# Makefile para Estudiantes - Simulador Roomba
#
# Uso:
#   make        - Compilar el proyecto
#   make run    - Compilar y ejecutar
#   make clean  - Limpiar archivos generados
#   make help   - Mostrar esta ayuda

CC = gcc
CFLAGS = -Wall -Wextra -lm -O2
TARGET = roomba

# ============================================================================
# Targets principales
# ============================================================================

# Compilar proyecto del estudiante
all: $(TARGET)

$(TARGET): main.c simula.o simula.h
	$(CC) main.c simula.o $(CFLAGS) -o $(TARGET)
	@echo ""
	@echo "=========================================="
	@echo "  Compilacion exitosa"
	@echo "=========================================="
	@echo "Ejecuta con:"
	@echo "  ./$(TARGET)                    # Mapa aleatorio"
	@echo "  ./$(TARGET) MAP=maps/noobs.pgm # Mapa especifico"
	@echo ""

# Ejecutar simulacion
run: $(TARGET)
	./$(TARGET)

# Limpiar archivos generados
clean:
	rm -f $(TARGET)
	rm -f *.csv *.pgm
	@echo "Archivos de compilacion y simulacion eliminados"

# ============================================================================
# Ayuda
# ============================================================================

help:
	@echo "=========================================="
	@echo "  Simulador Roomba - Comandos"
	@echo "=========================================="
	@echo ""
	@echo "Compilacion:"
	@echo "  make        - Compilar el proyecto"
	@echo "  make run    - Compilar y ejecutar"
	@echo "  make clean  - Limpiar archivos generados"
	@echo ""
	@echo "Archivos importantes:"
	@echo "  main.c      - Tu codigo (EDITAR AQUI)"
	@echo "  simula.o    - Biblioteca precompilada (NO MODIFICAR)"
	@echo "  simula.h    - API disponible (CONSULTAR)"
	@echo "  maps/       - Mapas de prueba"
	@echo ""
	@echo "Ejecutar con mapa especifico:"
	@echo "  ./roomba MAP=maps/noobs.pgm"
	@echo "  ./roomba MAP=maps/random3.pgm"
	@echo "  ./roomba MAP=maps/walls2.pgm"
	@echo ""
	@echo "Archivos generados tras ejecucion:"
	@echo "  log.csv     - Trayectoria del robot"
	@echo "  stats.csv   - Estadisticas finales"
	@echo "  map.pgm     - Imagen del mundo"
	@echo ""

.PHONY: all run clean help
