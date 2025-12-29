# Carpeta para mapas temporales
MAPS ?= maps

# Makefile para tests unitarios del simulador Roomba

CC = gcc
CFLAGS = -Wall -Wextra -Wno-unused-result -I../ -I../tools -g
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
TESTS = test_api
MAPGEN = mapgen


# Compila y crea carpeta de mapas temporales
all: $(TESTS) $(MAPGEN)
	mkdir -p $(MAPS)
# Compila el generador de mapas
$(MAPGEN): mapgen.c
	$(CC) $(CFLAGS) -o $@ $<

# Ejemplo de test unitario
test_api: test_api.o $(SIM_SRC) $(UNITY_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@



# Limpieza de binarios y mapas temporales
clean:
	rm -f *.o $(TESTS) $(MAPGEN)
	rm -rf $(MAPS)
