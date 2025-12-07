# Migración a Interfaz con Contexto en Simulador Roomba

## ¿Por qué usar un contexto?

- Permite que cada robot o simulación tenga su propio estado, evitando variables globales.
- Facilita la modularidad, reutilización y pruebas.
- Permite simular varios robots independientes.
- Evita errores por colisiones de nombres y efectos colaterales.
- Es más profesional y escalable.

## ¿Cómo se haría?

### 1. Definir el contexto
En el header principal, añade:
```c
typedef struct RobotContext RobotContext; // El usuario define los campos
```

### 2. Modificar la interfaz de configure
Cambia la firma de configure para aceptar funciones tipo `void (*)(void *)` y un puntero de contexto:
```c
void configure(void (*start)(void *), void (*beh)(void *), void (*stop)(void *), int exec_time, void *context);
```

### 3. Actualizar el ciclo principal y las llamadas
Modifica el código que llama a las funciones de usuario para pasar el puntero de contexto en cada llamada:
```c
start(context);
beh(context);
stop(context);
```

### 4. Actualizar ejemplos y documentación
Proporciona ejemplos de uso con contexto y explica la migración en el README/manual.

### 5. Mantener compatibilidad (opcional)
Puedes mantener una versión legacy de configure para no romper los programas antiguos de inmediato.

### 6. Probar y validar
Compila y prueba con varios ejemplos, asegurando que el contexto funciona y no hay variables globales necesarias.

## Ejemplo de uso
```c
typedef struct {
    int pasos;
    int colisiones;
} RobotContext;

void inicializar(void *ctx) {
    RobotContext *c = (RobotContext*)ctx;
    c->pasos = 0;
    c->colisiones = 0;
    int x, y;
    rmb_awake(&x, &y);
}

void comportamiento(void *ctx) {
    RobotContext *c = (RobotContext*)ctx;
    if (rmb_bumper()) {
        rmb_turn(M_PI / 2);
        c->colisiones++;
    } else {
        rmb_forward();
        c->pasos++;
    }
}

void finalizar(void *ctx) {
    RobotContext *c = (RobotContext*)ctx;
    printf("Pasos: %d, Colisiones: %d\n", c->pasos, c->colisiones);
    visualize();
}

int main() {
    RobotContext ctx;
    configure(inicializar, comportamiento, finalizar, 1000, &ctx);
    run();
    return 0;
}
```

---

**Recomendación:**
Haz estos cambios en una rama nueva (por ejemplo, `feature/context`) para no afectar a los usuarios actuales hasta que esté todo probado y documentado.