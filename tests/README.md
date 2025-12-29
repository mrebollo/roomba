# Tests Unitarios del Simulador Roomba

Este directorio contiene los tests unitarios para verificar el correcto funcionamiento de la API del simulador y sus componentes internos.

## Framework

Se utiliza **[Unity Test Framework](http://www.throwtheswitch.org/unity)**, un framework ligero de testing para C, ideal para sistemas embebidos y proyectos académicos.

## Ejecución

### Desde la raíz del proyecto (Recomendado)
```bash
make tests
```

### Desde el directorio `tests/`
```bash
make
./test_api
```

## Estructura

- **`test_api.c`**: Contiene todos los casos de prueba (test cases). Aquí es donde se añaden nuevos tests.
- **`unity.c` / `unity.h`**: El núcleo del framework Unity.
- **`mapgen.c`**: Utilidad auxiliar para generar mapas temporales necesarios para algunos tests.
- **`Makefile`**: Script de compilación específico para los tests.

## Cómo añadir nuevos tests

1.  Abre el archivo `test_api.c`.
2.  Define una nueva función de test con el prefijo `test_`, por ejemplo: `void test_mi_nueva_funcionalidad(void) { ... }`.
3.  Usa los macros de aserción de Unity, como `TEST_ASSERT_EQUAL_INT(exp, act)` o `TEST_ASSERT_TRUE(cond)`.
4.  Registra el test en la función `main` usando `RUN_TEST(test_mi_nueva_funcionalidad);`.

Ejemplo:
```c
void test_robot_avanza(void) {
    // Configuración
    int x, y;
    rmb_awake(&x, &y);

    // Acción
    rmb_forward();

    // Verificación
    sensor_t s = rmb_state();
    TEST_ASSERT_EQUAL_INT(x + 1, s.x);
}
```
