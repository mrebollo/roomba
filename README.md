# roomba
roomba simulator for IPR-GIIROB-ETSINF-UPV

## Quick Start
- Build and run the project:
	- `make clean`
	- `make`
	- `./roomba`

- Run with a specific map:
	- `make run-map MAP=maps/sample_map.pgm`

- Generated files:
	- `log.csv`: robot trajectory
	- `stats.csv`: execution statistics
	- `map.pgm`: world snapshot (PGM image)

Tip: Press Ctrl-C during visualization to exit early.

## Map Tools

### Generate test maps:
```bash
make mapgen       # Compile map generator
./maps/generate   # Generate 8 test maps
```

### Visualize maps (without running simulation):
```bash
make viewmap                    # Compile map viewer
./maps/viewmap maps/walls2.pgm  # View a specific map
```

The viewer displays maps in ASCII format:
- `#` = Walls/obstacles
- `B` = Robot base
- `1`-`5` = Dirt level
- Spaces = Empty cells

See `maps/README.md` for more details.
