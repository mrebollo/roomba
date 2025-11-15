# roomba
roomba simulator for IPR-GIIROB-ETSINF-UPV

## Quick Start
- Build and run the student project:
	- `cd /Users/mrebollo/devel/roomba/2025`
	- `make clean`
	- `make`
	- `./roomba`

- Run with a specific map:
	- `make run-map MAP=maps/sample_map.pgm`

- Use the student template (auto-shows results and optional visualization):
	- `gcc main_template.c simula.c -Wall -Wextra -O2 -lm -o roomba_template`
	- `./roomba_template`

- Generated files:
	- `log.csv`: robot trajectory
	- `stats.csv`: execution statistics
	- `map.pgm`: world snapshot (PGM image)

Tip: Press Ctrl-C during visualization to exit early.
