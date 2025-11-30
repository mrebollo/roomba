#ifndef _SIMULA_H
#define _SIMULA_H

/**
 * @brief Estructura con las lecturas de los sensores en el instante actual 
 * 
 */
typedef struct sensor{
    int x, y;           ///< Posición del robot (x: columna, y: fila)
    float heading;      ///< Orientación en radianes (0 = Este, PI/2 = Norte)
    int bumper;         ///< 1 = obstáculo detectado, 0 = libre
    int infrared;       ///< Nivel de suciedad detectado (0 = limpio, >0 = sucio)
    float battery;      ///< Batería disponible (máximo: 1000)
} sensor_t;

/**
 * @brief Función que ejecuta el comportamiento principal del robot.
 * 
 * El usuario debe crear una función de perfil void func(void) que contenga el código 
 * de un ciclo del robot. Esta función la ejecuta hasta que alcanza el tiempo de simulación 
 * o cualquier otra condición de terminación.
 * 
 */
void run();


/**
 * @brief Configura el simulador
 * 
 * La función recibe los nombres de tres funciones
 * - start, que se ejecuta una sola vez para preparar el simulador
 * - beh, que define el comportamiento principal del robot (cíclico)
 * - stop, que se ejecuta uan vez al final para ejecutar las tareas de cierre. 
 */
void configure(void (*start)(), void (*beh)(), void (*stop)(), int exec_time);

/**
 * @brief Carga un mapa generado en una simulación anterior
 * 
 * La función recibe el nombre del fichero (en formato PGM) que contiene
 * los obstáculos, la suciedad y la posición de la base.
 * 
 * @param filename Ruta del archivo PGM con el mapa
 * @return 0 si se cargó correctamente, -1 en caso de error
 */
int load_map(char *filename);


/**
 * @brief Visualiza la simulación 
 * 
 * Muestra el mapa completo del entorno, la suciedad de las casillas
 * y la posición del robot en cada momento, dejando marcas sobre la trayectoria que ha seguido.
 * Una barra de progreso indica la carga de la batería y se visualiza el estado del bumper 
 * (detección de obstáculos) y del sensor de infrarrojos (detección de suciedad).
 */
void visualize();


/**
 * @brief Activa el robot y resetea los sensores
 * 
 * El robot dispone de una base en la que reposa cuando está inactivo. Para activar al robot,
 * debe ejecutarse esta acción awake() cuando se encuentre en la posición de la base. 
 * La base siempre estará pegada a un muro y el ángulo inicial del robot será perpendicular a él.
 * 
 * El método devuelve a través de los parámetros las coordenadas x e y de la base.
 * 
 * Esta acción no consume batería.
 * 
 * @param [out] x Coordenada horizontal de la posición de la base (columna)
 * @param [out] y Coordenada vertical de la posición de la base (fila)
 * @return 1 si el robot se ha activado con éxito, 0 en cualquier otro caso
 */
int rmb_awake(int *x, int *y);

/**
 * @brief Acción de girar el robot un ángulo 'alpha' respecto a la orientación actual
 * 
 * El ángulo está expresado en radianes. Puede ser un valor negativo.  
 * Esta acción consume 0.1 unidades de batería.
 * 
 * @param alpha Ángulo que debe girar el robot (en radianes)
 */
void rmb_turn(float alpha);

/**
 * @brief Acción de avanzar un paso en la dirección en la que apunta actualmente
 * 
 * Si detecta un obstáculo, se activarán los bumpers y no se cambiará la posición. 
 * Esta acción consume:
 *  - 1 unidad de batería si se mueve recto
 *  - 1.4 unidades de batería si se mueve en diagonal
 */
void rmb_forward();

/**
 * @brief Acción de limpiar la posición actual
 * 
 * Elimina una unidad de suciedad de la posición donde se encuentra el robot.
 * Esta acción consume 0.5 unidades de batería.
 */
void rmb_clean();

/**
 * @brief Acción de realizar un ciclo de carga en la batería
 * 
 * Cada ciclo de carga restaura 10 unidades de batería sin sobrepasar el máximo (1000).
 * Para poder ejecutar esta acción, el robot debe encontrarse en la posición de la base.
 * 
 * Esta acción no consume batería.
 * 
 * @return 1 si el ciclo de recarga se ha completado con éxito, 0 en otro caso
 */
int rmb_load();

/**
 * @brief Consulta los valores de todos los sensores al mismo tiempo
 * 
 * @return sensor_t Estructura con los valores actuales de todos los sensores
 */
sensor_t rmb_state();

/**
 * @brief Consulta el valor del bumper
 * 
 * Devuelve 1 si está pulsado (detecta obstáculo) o 0 en cualquier otro caso.
 * 
 * @return 1 si hay obstáculo, 0 si está libre
 */
int rmb_bumper();

/**
 * @brief Consulta el valor del sensor de infrarrojos
 * 
 * Indica el nivel de suciedad de la posición actual.
 * 
 * @return Nivel de suciedad (>0 si hay suciedad, 0 si está limpio)
 */
int rmb_ifr();

/**
 * @brief Consulta el nivel de batería
 * 
 * @return Nivel actual de batería (máximo: 1000, mínimo: 0)
 */
float rmb_battery();

/**
 * @brief Consulta si el robot se encuentra en la base
 * 
 * @return 1 si el robot está en la base, 0 si está en otra posición
 */
int rmb_at_base();

#endif