/**
 * @file sim_world.c
 * @brief Gestión del mundo/mapa de simulación
 * 
 * Contiene funciones para generar, cargar, guardar y consultar
 * el estado del mapa (obstáculos, suciedad, base).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "simula_internal.h"

#define PGM_WALL_VALUE 128            ///< Valor de gris para obstáculos en PGM
#define PGM_EMPTY_VALUE 255           ///< Valor de gris para celdas vacías en PGM
#define PGM_BASE_VALUE 0              ///< Valor de gris para la base en PGM
#define BORDER_MARGIN 2               ///< Margen desde el borde para generar obstáculos
#define WALL_MIN_OFFSET 4             ///< Offset mínimo para posicionar muros
#define LINE_BUFFER_SIZE 256          ///< Tamaño del buffer para leer líneas

/* ============================================================================
 * FUNCIONES AUXILIARES PRIVADAS
 * ============================================================================ */

/**
 * @brief Calcula la orientación apropiada para la base según su posición
 * @param m Puntero al mapa
 * @param x Columna de la base
 * @param y Fila de la base
 * @return Ángulo en radianes (apunta hacia el centro del mapa)
 */
static float base_heading(const map_t* m, int x, int y){
  float head[4] = {M_PI / 2, 3 * M_PI / 2, 0, M_PI};
  if(x == 1) return head[2];
  else if(x == m->ncol-2) return head[3];
  else if(y == 1) return head[0];
  else return head[1];
}

/* ============================================================================
 * API PÚBLICA DE MUNDO
 * ============================================================================ */

/**
 * @brief Coloca la base en una posición específica del mapa
 * @param m Puntero al mapa
 * @param x Columna donde colocar la base
 * @param y Fila donde colocar la base
 * @return Orientación apropiada en radianes
 */
float sim_world_put_base(map_t* m, int x, int y){
  m->cells[y][x] = 'B';
  m->base_x = x;
  m->base_y = y;
  return base_heading(m, x, y);
}

/**
 * @brief Establece la posición inicial del robot en la base
 * @param m Puntero al mapa
 * @param x Puntero donde guardar la columna inicial
 * @param y Puntero donde guardar la fila inicial
 * @param h Puntero donde guardar la orientación inicial
 * 
 * Por defecto coloca la base en (1,1) mirando al este (0 radianes).
 */
void sim_world_set_base_origin(map_t* m, int *x, int *y, float *h){
  if(!x || !y || !h){
    fprintf(stderr, "Error: Invalid parameters for sim_world_set_base_origin\n");
    return;
  }
  *x = 1;
  *y = 1;
  *h = 0;
  m->cells[*y][*x] = 'B';
}

/**
 * @brief Guarda el mapa actual en formato PGM
 * @param m Puntero al mapa
 * 
 * Genera un archivo map.pgm en el directorio actual con el estado
 * del mapa: obstáculos (128), celdas vacías (255), base (0),
 * y suciedad (1-MAXDIRT).
 */
void sim_world_save(const map_t* m){
  FILE *fd = fopen("map.pgm","w");
  if(!fd){
    fprintf(stderr, "Error: Cannot open map.pgm for writing\n");
    return;
  }
  fprintf(fd,"P2\n#roomba map\n%d %d\n%d\n", m->ncol, m->nrow, PGM_EMPTY_VALUE);
  for(int i = 0; i < m->nrow; i++){
    for(int j = 0; j < m->ncol; j++){
      switch(m->cells[i][j]){
        case WALL: fprintf(fd,"%d ", PGM_WALL_VALUE); break;
        case EMPTY: fprintf(fd,"%d ", PGM_EMPTY_VALUE); break;
        case 'B': fprintf(fd,"%d ", PGM_BASE_VALUE); break;
        default: fprintf(fd,"%d ", m->cells[i][j]-DIGIT_CHAR_BASE);
      }
    }
    fprintf(fd,"\n");
  }
  fclose(fd);
}

/**
 * @brief Verifica si una celda es un obstáculo
 * @param m Puntero al mapa
 * @param y Fila
 * @param x Columna
 * @return 1 si es obstáculo o fuera de límites, 0 si libre
 */
int sim_world_is_wall(const map_t* m, int y, int x){
  if(y < 0 || y >= m->nrow || x < 0 || x >= m->ncol)
    return 1; // Out of bounds treated as wall
  return m->cells[y][x] == WALL;
}

/**
 * @brief Verifica si una celda es la base
 * @param m Puntero al mapa
 * @param y Fila
 * @param x Columna
 * @return 1 si es la base, 0 si no
 */
int sim_world_is_base(const map_t* m, int y, int x){
  if(y < 0 || y >= m->nrow || x < 0 || x >= m->ncol)
    return 0;
  return m->cells[y][x] == 'B';
}

/**
 * @brief Obtiene el nivel de suciedad de una celda
 * @param m Puntero al mapa
 * @param y Fila
 * @param x Columna
 * @return Nivel de suciedad (0 si limpia, fuera de límites o especial)
 */
int sim_world_cell_dirt(const map_t* m, int y, int x){
  if(y < 0 || y >= m->nrow || x < 0 || x >= m->ncol)
    return 0;
  char c = m->cells[y][x];
  if(c == 'B' || c == WALL || c == EMPTY) return 0;
  return c - DIGIT_CHAR_BASE;
}

/**
 * @brief Establece el nivel de suciedad de una celda
 * @param m Puntero al mapa
 * @param y Fila
 * @param x Columna
 * @param dirt Nivel de suciedad (0 = limpia)
 */
void sim_world_set_cell_dirt(map_t* m, int y, int x, int dirt){
  if(y < 0 || y >= m->nrow || x < 0 || x >= m->ncol)
    return;
  if(dirt <= 0) m->cells[y][x] = EMPTY;
  else m->cells[y][x] = (char)(DIGIT_CHAR_BASE + dirt);
}

/**
 * @brief Limpia una celda reduciendo su suciedad en 1
 * @param m Puntero al mapa
 * @param y Fila
 * @param x Columna
 * @return Nuevo nivel de suciedad después de limpiar
 */
int sim_world_clean_cell(map_t* m, int y, int x){
  int d = sim_world_cell_dirt(m, y, x);
  if(d > 0){
    --d;
    sim_world_set_cell_dirt(m, y, x, d);
  }
  return d;
}

/* ============================================================================
 * GENERACIÓN DE MAPAS ALEATORIOS
 * ============================================================================ */

/**
 * @brief Crea un muro vertical aleatorio
 * @param m Puntero al mapa
 */
static void create_vertical_wall(map_t* m){
  int len = rand()%m->nrow/2 + m->nrow/4;
  int init = rand()%m->nrow/2 + BORDER_MARGIN;
  int col = rand()%(m->ncol-WALL_MIN_OFFSET)+BORDER_MARGIN;
  for(int i = 0; i < len; i++)
    m->cells[init + i][col] = WALL;
}

/**
 * @brief Crea un muro horizontal aleatorio
 * @param m Puntero al mapa
 */
static void create_horiz_wall(map_t* m){
  int len = rand()%m->ncol/2 + m->ncol/4;
  int init = rand()%m->ncol/2 + BORDER_MARGIN;
  int row = rand()%(m->nrow-WALL_MIN_OFFSET)+BORDER_MARGIN;
  for(int i = 0; i < len; i++)
    m->cells[row][init + i] = WALL;
}

/**
 * @brief Crea obstáculos aleatorios dispersos
 * @param m Puntero al mapa
 * @param prop Proporción de celdas que serán obstáculos (0.0 a 1.0)
 */
static void create_random_obstacles(map_t* m, float prop){
  for(int i = BORDER_MARGIN; i < WORLDSIZE-BORDER_MARGIN; i++)
    for(int j = BORDER_MARGIN; j < WORLDSIZE-BORDER_MARGIN; j++)
      if(rand()/(float)RAND_MAX < prop){
        m->cells[i][j] = WALL;
        DEBUG_PRINT("%d, %d\n", i, j);
      }
}

/**
 * @brief Inicializa un mapa vacío
 * @param m Puntero al mapa
 * @param nrow Número de filas
 * @param ncol Número de columnas
 */
static void init_empty_world(map_t* m, int nrow,int ncol){
  m->nrow = nrow;
  m->ncol = ncol;
  for(int i = 0; i < WORLDSIZE; i++)
    for(int j = 0; j < WORLDSIZE; j++)
      m->cells[i][j] = EMPTY;
}

/**
 * @brief Añade muros perimetrales al mapa
 * @param m Puntero al mapa
 */
static void add_border_walls(map_t* m){
  for(int i = 0; i < m->nrow; i++){
    m->cells[i][0] = WALL;
    m->cells[i][m->ncol-1] = WALL;
  }
  for(int i = 0; i < m->ncol; i++){
    m->cells[0][i] = WALL;
    m->cells[m->nrow-1][i] = WALL;
  }
}

/**
 * @brief Añade obstáculos al mapa según densidad
 * @param m Puntero al mapa
 * @param nobs Densidad de obstáculos (>1 = número de muros, <1 = proporción)
 */
static void add_obstacles(map_t* m, float nobs){
  if(nobs >= 1){
    int numobs = (int)nobs;
    while(numobs-- > 0){
      if(rand()%2) create_vertical_wall(m);
      else create_horiz_wall(m);
    }
  } else if(nobs > 0) {
    create_random_obstacles(m, nobs);
  }
}

/**
 * @brief Coloca celdas sucias aleatoriamente en el mapa
 * @param m Puntero al mapa
 * @param num_dirty Número de celdas a ensuciar
 */
static void place_dirt(map_t* m, int num_dirty){
  m->ndirt = num_dirty;
  for(int i = 0; i < num_dirty; i++){
    int row, col;
    do{
      row = rand()%(m->nrow-2) + 1;
      col = rand()%(m->ncol-2) + 1;
    }while(m->cells[row][col] != EMPTY);
    m->dirt[i].x = col;
    m->dirt[i].y = row;
    m->dirt[i].depth = rand()%MAXDIRT + 1;
    m->cells[row][col] = DIGIT_CHAR_BASE + m->dirt[i].depth;
  }
}

/**
 * @brief Genera un mapa aleatorio
 * @param m Puntero al mapa
 * @param nrow Número de filas
 * @param ncol Número de columnas
 * @param num_dirty Número de celdas sucias
 * @param nobs Densidad de obstáculos
 * @return 0 si OK, -1 si dimensiones inválidas
 */
int sim_world_generate(map_t* m, int nrow, int ncol, int num_dirty, float nobs){
  if(nrow > WORLDSIZE || ncol > WORLDSIZE)
    return -1;
  init_empty_world(m, nrow, ncol);
  add_border_walls(m);
  add_obstacles(m, nobs);
  place_dirt(m, num_dirty);
  return 0;
}

/* ============================================================================
 * CARGA DE MAPAS DESDE ARCHIVOS PGM
 * ============================================================================ */

/**
 * @brief Lee la cabecera de un archivo PGM
 * @param fd File descriptor abierto
 * @param nrow Puntero donde guardar número de filas
 * @param ncol Puntero donde guardar número de columnas
 * @return 0 si OK, -1 si error
 */
static int read_pgm_header(FILE *fd, int *nrow, int *ncol){
  char line[LINE_BUFFER_SIZE];
  int aux;
  
  fgets(line, LINE_BUFFER_SIZE, fd);  // P2
  fgets(line, LINE_BUFFER_SIZE, fd);  // comment
  fscanf(fd, "%d%d", ncol, nrow);
  fscanf(fd, "%d", &aux);
  
  if(*nrow > WORLDSIZE || *ncol > WORLDSIZE){
    fprintf(stderr, "Error: Map size (%dx%d) exceeds WORLDSIZE\n", *nrow, *ncol);
    return -1;
  }
  return 0;
}

/**
 * @brief Procesa una celda individual del archivo PGM
 * @param m Puntero al mapa
 * @param cell Valor de gris de la celda
 * @param i Fila
 * @param j Columna
 * @param dc Puntero al contador de celdas sucias
 */
static void process_cell(map_t *m, int cell, int i, int j, int *dc){
  switch(cell){
    case PGM_WALL_VALUE: 
      m->cells[i][j] = WALL; 
      break;
    case PGM_EMPTY_VALUE: 
      m->cells[i][j] = EMPTY; 
      break;
    case PGM_BASE_VALUE: 
      sim_world_put_base(m, j, i); 
      break;
    default:
      m->dirt[*dc].x = j;
      m->dirt[*dc].y = i;
      m->dirt[*dc].depth = cell;
      (*dc)++;
      m->cells[i][j] = cell + DIGIT_CHAR_BASE;
      m->ndirt++;
      break;
  }
}

/**
 * @brief Lee y parsea todas las celdas del mapa desde el archivo
 * @param fd File descriptor abierto
 * @param m Puntero al mapa donde guardar los datos
 * @return 0 si OK
 */
static int parse_map_cells(FILE *fd, map_t *m){
  char line[LINE_BUFFER_SIZE];
  int dc = 0;
  
  for(int i = 0; i < m->nrow; i++){
    for(int j = 0; j < m->ncol; j++){
      int cell;
      fscanf(fd, "%d", &cell);
      process_cell(m, cell, i, j, &dc);
    }
    fgets(line, LINE_BUFFER_SIZE, fd);
  }
  return 0;
}

/**
 * @brief Carga un mapa desde un archivo PGM
 * @param m Puntero al mapa
 * @param filename Ruta del archivo PGM
 * @return 0 si OK, -1 si error
 * 
 * Lee un archivo en formato PGM (P2) con el mapa de obstáculos,
 * suciedad y base. Actualiza la estructura del mapa con los datos.
 */
int sim_world_load(map_t* m, char *filename){
  int nrow, ncol;
  
  DEBUG_PRINT("Loading map %s\n", filename);
  FILE *fd = fopen(filename, "r");
  if(!fd){
    fprintf(stderr, "Error: Cannot open %s for reading\n", filename);
    return -1;
  }
  
  m->ndirt = 0;
  if(read_pgm_header(fd, &nrow, &ncol) != 0){
    fclose(fd);
    return -1;
  }
  m->nrow = nrow;
  m->ncol = ncol;  
  parse_map_cells(fd, m);
  fclose(fd);  
  if(filename)
    snprintf(m->name, sizeof(m->name), "%s", filename);
  else
    m->name[0] = '\0';
  return 0;
}
