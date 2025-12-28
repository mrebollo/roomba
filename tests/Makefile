# Carpeta para mapas temporales
MAPS ?= maps

# Makefile para tests unitarios del simulador Roomba

CC = gcc
CFLAGS = -Wall -Wextra -I../ -I../tools -g
LDFLAGS = -lm

# Archivos fuente del simulador necesarios para los tests
SIM_SRC = \
	../simula.c \
	../sim_world.c \
	../sim_world_api.c \
	../sim_robot.c \
	../sim_stats.c \
	../sim_io.c \
	../sim_visual.c


# Archivos de test y Unity
UNITY_SRC = unity.c
UNITY_HDR = unity.h unity_internals.h
TESTS = test_simula
MAPGEN = mapgen


# Compila y crea carpeta de mapas temporales
all: $(TESTS) $(MAPGEN)
	mkdir -p $(MAPS)
# Compila el generador de mapas
$(MAPGEN): mapgen.c
	$(CC) $(CFLAGS) -o $@ $<

# Ejemplo de test unitario
test_simula: test_simula.o $(SIM_SRC) $(UNITY_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@



# Limpieza de binarios y mapas temporales
clean:
	rm -f *.o $(TESTS) $(MAPGEN)
	rm -rf $(MAPS)
# Ejemplo de uso de la variable de entorno en un test o script:
#
#   make MAP_TMP_DIR=maps_tmp
#   ./mapgen $(MAP_TMP_DIR)/testmap.pgm 8 1 1
#   ./test_simula
#
# O en bash:
#   export MAP_TMP_DIR=maps_tmp
#   ./mapgen "$MAP_TMP_DIR/testmap.pgm" 8 1 1
