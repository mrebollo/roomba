#!/usr/bin/env python3
"""Generate `results/teamXX/mapX_stats.csv` from per-team `stats.csv` files.

This post-processing script is useful because the runner stores per-run
lines in `competition/teams/teamXX/stats.csv`. The organizer docs expect
`competition/results/teamXX/mapX_stats.csv` and `all_stats.csv`.

It uses the same defaults as the runner: MAPS_COUNT=4, REPS_PER_MAP=5
unless overridden by environment variables.
"""
import os
from pathlib import Path
import shutil

ROOT = Path(__file__).resolve().parents[1]
TEAMS_DIR = ROOT / 'teams'
RESULTS_DIR = ROOT / 'results'

# Defaults matching runner.c
MAPS_COUNT = int(os.environ.get('MAPS_COUNT', '4'))
REPS_PER_MAP = int(os.environ.get('REPS_PER_MAP', '5'))

def ensure(p: Path):
    p.mkdir(parents=True, exist_ok=True)

def process_team(team_path: Path):
    stats_file = team_path / 'stats.csv'
    if not stats_file.exists():
        return False

    team_name = team_path.name
    target_dir = RESULTS_DIR / team_name
    ensure(target_dir)

    # Prepare per-map files
    map_files = [open(target_dir / f'map{m}_stats.csv', 'w') for m in range(MAPS_COUNT)]
    # write header for map files (use same header as runner's teams/stats.csv)
    header = 'cell_total, cell_visited, dirt_total, dirt_cleaned, bat_total, bat_mean, forward, turn, bumps, clean, load\n'
    for f in map_files:
        f.write(header)

    # Read team stats (skip header) and split into maps
    with open(stats_file, 'r') as fh:
        lines = fh.read().splitlines()
    if not lines:
        for f in map_files:
            f.close()
        return False

    # skip header
    data_lines = lines[1:]
    for i, line in enumerate(data_lines):
        map_id = (i // REPS_PER_MAP) % MAPS_COUNT
        map_files[map_id].write(line.rstrip() + '\n')

    for f in map_files:
        f.close()

    # create all_stats.csv concatenating map files
    all_path = target_dir / 'all_stats.csv'
    with open(all_path, 'w') as out:
        out.write('team,map_type,' + header)
        for m in range(MAPS_COUNT):
            mf = target_dir / f'map{m}_stats.csv'
            if not mf.exists():
                continue
            with open(mf, 'r') as inh:
                next(inh, None)  # skip header
                for l in inh:
                    out.write(f"{team_name},{m},{l}")

    return True

def main():
    ensure(RESULTS_DIR)
    teams = [p for p in TEAMS_DIR.iterdir() if p.is_dir()]
    created = 0
    for t in teams:
        ok = process_team(t)
        if ok:
            created += 1
            print(f"Processed {t.name} -> results/{t.name}/")
    print(f"Done. Created results for {created} team(s) in {RESULTS_DIR}")

if __name__ == '__main__':
    main()
