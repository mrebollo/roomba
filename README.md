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
