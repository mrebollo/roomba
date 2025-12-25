# Makefile Standalone - Simulador Roomba
#
# Uso:
#   make        - Compilar el proyecto
#   make run    - Compilar y ejecutar
#   make clean  - Limpiar archivos generados
#   make help   - Mostrar ayuda

CC = gcc
CFLAGS = -Wall -Wextra -lm
TARGET = roomba

# Compilar proyecto principal
all: $(TARGET)

$(TARGET): main.c simula.o simula.h
	$(CC) main.c simula.o $(CFLAGS) -o $(TARGET)
	@echo "Compilacion exitosa. Ejecuta con: ./$(TARGET)"

# Ejecutar simulacion
run: $(TARGET)
	./$(TARGET)

# Compilar herramientas
tools:
	@$(MAKE) -C tools
	@echo "Herramientas compiladas en tools/"

# Generar nuevos mapas
maps:
	@$(MAKE) -C tools generate
	@./tools/generate
	@echo "Nuevos mapas generados en maps/"

# Limpiar archivos generados
clean:
	rm -f $(TARGET)
	rm -f *.csv *.pgm
	@$(MAKE) -C tools clean 2>/dev/null || true
	@echo "Archivos limpiados (usar 'make clean-all' para borrar mapas)"

# Limpieza total (incluyendo mapas)
clean-all: clean
	rm -f maps/*.pgm
	@echo "Mapas eliminados"

# Ayuda
help:
	@echo "Simulador Roomba - Comandos disponibles:"
	@echo ""
	@echo "  make          - Compilar el proyecto"
	@echo "  make run      - Compilar y ejecutar"
	@echo "  make tools    - Compilar herramientas (myscore, validate...)"
	@echo "  make maps     - Generar nuevos mapas de prueba"
	@echo "  make clean    - Limpiar archivos generados"
	@echo "  make clean-all - Limpiar todo (incluyendo mapas)"
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
