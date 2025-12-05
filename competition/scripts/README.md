# Competition Scripts

Scripts auxiliares para gestionar la competición de Roombas.

## Scripts disponibles

### setup.sh
Configura el entorno de competición completo:
- Crea estructura de directorios
- Construye la biblioteca de competición
- Sincroniza headers
- Compila el runner
- Valida equipos

**Uso:**
```bash
cd competition/scripts
./setup.sh
```

## Scripts adicionales sugeridos

### compile_all.sh (TODO)
Compila todos los equipos sin ejecutar la competición.

```bash
#!/bin/bash
cd competition
for team in teams/*/; do
    make test TEAM=$(basename "$team")
done
```

### analyze_results.py (TODO)
Análisis estadístico de resultados:
- Gráficos de rendimiento
- Comparativa entre equipos
- Tendencias por mapa
- Detección de outliers

### clean_all.sh (TODO)
Limpieza profunda de todos los artefactos:
```bash
#!/bin/bash
cd competition
make clean-all
rm -rf results/*
```

### backup_results.sh (TODO)
Crea backup comprimido de todos los resultados:
```bash
#!/bin/bash
TIMESTAMP=$(date +%Y%m%d_%H%M%S)
tar -czf "competition_backup_$TIMESTAMP.tar.gz" results/
```

## Contribuir

Para añadir nuevos scripts:
1. Crear script en este directorio
2. Hacer ejecutable: `chmod +x script.sh`
3. Documentar en este README
4. Añadir comentarios en el script
