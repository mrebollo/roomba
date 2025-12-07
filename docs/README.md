# Documentación del Simulador Roomba 2025

Este directorio contiene toda la documentación generada para el simulador de Roomba.

## Estructura de Documentación

```
docs/
├── html/             Documentación de la API (Doxygen)
├── user/             Manual del usuario/estudiante
├── developer/        Manual técnico del desarrollador
├── Doxyfile          Configuración de Doxygen
├── mainpage.dox      Página principal de Doxygen
└── README.md         Este archivo
```

### 1. Documentación de la API (`html/`)
Documentación técnica completa generada con Doxygen.

- **Ubicación**: `html/index.html`
- **Generación**: Ejecutar `doxygen Doxyfile` desde el directorio `docs/`
- **Contenido**: 
  - Referencia completa de todas las funciones públicas e internas
  - Diagramas de estructuras de datos
  - Documentación de cada módulo (`simula.c`, `sim_robot.c`, `sim_world.c`, etc.)
  - Ejemplos de uso embebidos

**Para visualizar**: Abre `html/index.html` en tu navegador web.

### 2. Manual del Usuario (`user/`)
Guía paso a paso para programar un robot usando la API de `simula.h`.

#### Formatos disponibles:
- **Markdown**: `MANUAL_USUARIO.md` (visualización rápida en GitHub/editores)
- **LaTeX**: `manual_usuario.tex` (código fuente)
- **PDF**: `manual_usuario.pdf` (versión imprimible)

**Para regenerar el PDF**:
```bash
cd user/
pdflatex manual_usuario.tex
pdflatex manual_usuario.tex  # Segunda pasada para referencias
```

#### Contenido del manual:
1. Introducción al simulador
2. Estructura básica de un programa
3. Configuración del entorno
4. API de sensores (`rmb_bumper`, `rmb_ifr`, `rmb_battery`, `rmb_state`)
5. API de movimiento (`rmb_turn`, `rmb_forward`)
6. API de limpieza y recarga (`rmb_clean`, `rmb_load`, `rmb_at_base`)
7. Ejemplos completos:
   - Robot básico que avanza
   - Detección de obstáculos
   - Limpieza de suciedad
   - Recarga de batería
   - Movimiento en cuadrado
   - Exploración con comportamiento reactivo
   - Estrategia espiral
   - Vuelta a la base por batería baja

### 3. Manual del Desarrollador (`developer/`)
*(Pendiente de generación)*

Este manual incluirá:
- Arquitectura interna del simulador
- Detalles de implementación de cada módulo
- Gestión de memoria y estructuras de datos
- Sistema de coordenadas y orientación
- Algoritmos de generación de mapas
- Sistema de logging y estadísticas

## Archivos de Configuración

- **Doxyfile**: Configuración de Doxygen (proyecto "Simulador Roomba 2025")
- **mainpage.dox**: Página principal de la documentación Doxygen con introducción y ejemplos

## Regeneración Completa

Para regenerar toda la documentación:

```bash
# Desde el directorio docs/

# 1. Documentación de la API (genera html/)
doxygen Doxyfile

# 2. PDF del manual del usuario
cd user/
pdflatex manual_usuario.tex
pdflatex manual_usuario.tex
cd ..

# 3. PDF del manual del desarrollador (cuando esté creado)
cd developer/
pdflatex manual_desarrollador.tex
pdflatex manual_desarrollador.tex
cd ..
```

## Versión de las Herramientas

- **Doxygen**: 1.15.0
- **LaTeX**: pdfTeX (cualquier distribución moderna)
- **Código documentado**: Rama `main` (versión 2025)

## Notas

- Todos los archivos `.h` y `.c` del proyecto contienen comentarios Doxygen completos
- La documentación se actualiza automáticamente al ejecutar `doxygen Doxyfile`
- El manual del estudiante está optimizado para imprimir en A4
- Se recomienda revisar `docs/html/index.html` para la referencia más completa
