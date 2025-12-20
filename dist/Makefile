# Makefile simplificado para estudiantes - Simulador Roomba
#
# Uso:
#   make        - Compilar el proyecto
#   make run    - Compilar y ejecutar
#   make clean  - Limpiar archivos generados
#   make help   - Mostrar ayuda

CC = gcc
CFLAGS = -Wall -Wextra -lm
TARGET = roomba

# Compilar proyecto del estudiante
all: $(TARGET)

$(TARGET): main.c simula.o simula.h
	$(CC) main.c simula.o $(CFLAGS) -o $(TARGET)
	@echo "Compilacion exitosa. Ejecuta con: ./$(TARGET)"

# Ejecutar simulacion
run: $(TARGET)
	./$(TARGET)

# Limpiar archivos generados
clean:
	rm -f $(TARGET)
	rm -f *.csv *.pgm
	@echo "Archivos limpiados"

# Ayuda
help:
	@echo "Simulador Roomba - Comandos disponibles:"
	@echo ""
	@echo "  make          - Compilar el proyecto"
	@echo "  make run      - Compilar y ejecutar"
	@echo "  make clean    - Limpiar archivos generados"
	@echo "  make -C tools - Compilar herramientas adicionales"
	@echo "  make help     - Mostrar esta ayuda"
	@echo ""
	@echo "Archivos importantes:"
	@echo "  main.c    - Tu codigo (EDITAR AQUI)"
	@echo "  simula.o  - Biblioteca (no modificar)"
	@echo "  simula.h  - API disponible (consultar)"
	@echo ""
	@echo "Para usar un mapa especifico:"
	@echo "  ./roomba MAP=maps/noobs.pgm"
	@echo ""
	@echo "Archivos generados tras ejecutar:"
	@echo "  log.csv   - Trayectoria del robot"
	@echo "  stats.csv - Estadisticas finales"
	@echo "  map.pgm   - Imagen del mundo"

.PHONY: all run clean help
