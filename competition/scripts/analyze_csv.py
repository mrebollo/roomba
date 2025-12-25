import configparser
#!/usr/bin/env python3
"""
Analyze 24-25 CSV results with new scoring system
"""

import csv
import sys
from collections import defaultdict
import math

def calculate_new_scores(data, weights, bonuses, thresholds):
    """Apply new scoring formulas to CSV data"""
    
    # Group by team
    teams = defaultdict(lambda: {
        'maps': [],
        'coverage': [],
        'dirt_eff': [],
        'bat_cons': [],
        'movement': [],
        'crashes': 0
    })

    for row in data:
        team = row['team']

        # Parse values (handle European format: comma as decimal)
        cell_total = float(row['cell_total'])
        cell_visited = float(row['cell_visited'])
        dirt_cleaned = float(row['dirt_cleaned'])
        bat_total = float(row['bat_total'].replace(',', '.'))
        bat_mean = float(row['bat_mean'].replace(',', '.'))
        forward = int(row['forward'])
        turn = int(row['turn'])
        bumps = int(row['bumps'])

        # Calculate new scores
        coverage = (cell_visited / cell_total) * 100 if cell_total > 0 else 0
        dirt_eff = min(100, (dirt_cleaned / bat_total) * 100) if bat_total > 0.1 else 0
        bat_cons = min(100, (bat_mean / 1000) * 100)

        total_moves = forward + turn + bumps
        bump_ratio = bumps / total_moves if total_moves > 0 else 0
        movement = (1 - bump_ratio) * 100

        teams[team]['maps'].append({
            'coverage': coverage,
            'dirt_eff': dirt_eff,
            'bat_cons': bat_cons,
            'movement': movement
        })
        teams[team]['coverage'].append(coverage)
        teams[team]['dirt_eff'].append(dirt_eff)
        teams[team]['bat_cons'].append(bat_cons)
        teams[team]['movement'].append(movement)

        # Detect crashes
        if coverage < 1.0 and cell_visited < 5:
            teams[team]['crashes'] += 1

    # Calculate final scores
    results = []
    for team, stats in teams.items():
        if not stats['maps']:
            continue

        avg_coverage = sum(stats['coverage']) / len(stats['coverage'])
        avg_dirt_eff = sum(stats['dirt_eff']) / len(stats['dirt_eff'])
        avg_bat_cons = sum(stats['bat_cons']) / len(stats['bat_cons'])
        avg_movement = sum(stats['movement']) / len(stats['movement'])

        # Weighted score (from config)
        total_score = (
            avg_coverage * weights['coverage'] / 100.0 +
            avg_dirt_eff * weights['dirt_efficiency'] / 100.0 +
            avg_bat_cons * weights['battery_conservation'] / 100.0 +
            avg_movement * weights['movement_quality'] / 100.0
        )

        # Bonuses
        if stats['crashes'] == 0:
            total_score += bonuses['completion_bonus']
        else:
            total_score -= stats['crashes'] * bonuses['crash_penalty']

        # Low bumps bonus (threshold from config)
        if avg_movement > (1 - thresholds['low_bumps_threshold']) * 100:
            total_score += bonuses['low_bumps_bonus']

        total_score = max(0, total_score)

        results.append({
            'team': team,
            'score': total_score,
            'coverage': avg_coverage,
            'dirt_eff': avg_dirt_eff,
            'bat_cons': avg_bat_cons,
            'movement': avg_movement,
            'crashes': stats['crashes'],
            'num_maps': len(stats['maps'])
        })

    # Sort by score
    results.sort(key=lambda x: x['score'], reverse=True)

    return results

def calculate_old_scores(data):
    """Calculate old scoring from CSV (discovered, eff batt, effective)"""
    
    teams = defaultdict(lambda: {'global_scores': []})
    
    for row in data:
        team = row['team']
        try:
            # Parse old scores (handle quotes and European format)
            global_val = row.get('global', '').strip()
            if global_val:
                global_score = float(global_val.strip('"').replace(',', '.'))
                teams[team]['global_scores'].append(global_score)
        except Exception as e:
            # Debug: show first parsing error
            if not teams:
                print(f"Warning: Could not parse global score for {team}: '{row.get('global', 'N/A')}' - {e}")
            continue
    
    # Calculate averages (without normalization)
    results = []
    for team, stats in teams.items():
        if not stats['global_scores']:
            continue
        
        avg_global = sum(stats['global_scores']) / len(stats['global_scores'])
        results.append({
            'team': team,
            'old_score': avg_global
        })
    
    results.sort(key=lambda x: x['old_score'], reverse=True)
    return results

def main():
    if len(sys.argv) < 3:
        print("Usage: python3 analyze_csv.py <csv_file_1> <csv_file_2>")
        sys.exit(1)

    # Read both CSVs
    with open(sys.argv[1], 'r', encoding='utf-8') as f1:
        reader1 = csv.DictReader(f1)
        data1 = list(reader1)
    with open(sys.argv[2], 'r', encoding='utf-8') as f2:
        reader2 = csv.DictReader(f2)
        data2 = list(reader2)

    # Read scoring.conf
    config = configparser.ConfigParser()
    config.read('scoring.conf')
    def clean_float(val):
        return float(val.split('#')[0].strip())
    weights = {k: clean_float(v) for k, v in config['weights'].items()}
    bonuses = {k: clean_float(v) for k, v in config['bonuses'].items()}
    thresholds = {k: clean_float(v) for k, v in config['thresholds'].items()}

    print(f"Loaded {len(data1)} rows from {sys.argv[1]}")
    print(f"Loaded {len(data2)} rows from {sys.argv[2]}\n")

    # Calculate scores for both
    results1 = calculate_new_scores(data1, weights, bonuses, thresholds)
    results2 = calculate_new_scores(data2, weights, bonuses, thresholds)
    dict1 = {r['team']: r for r in results1}
    dict2 = {r['team']: r for r in results2}

    # Union de equipos
    all_teams = sorted(set(dict1.keys()) | set(dict2.keys()))

    print("═" * 100)
    print("COMPARISON: Score in File 1 vs File 2".center(100))
    print("═" * 100)
    print(f"{'Rank':<6}{'Team':<12}{'Score1':<12}{'Score2':<12}{'Diff':<10}{'Cover1':<8}{'Cover2':<8}{'DirtEff1':<10}{'DirtEff2':<10}{'BatCons1':<10}{'BatCons2':<10}{'Move1':<8}{'Move2':<8}")
    print("─" * 100)

    ranked = sorted(all_teams, key=lambda t: dict2.get(t, {'score': 0})['score'], reverse=True)
    for i, team in enumerate(ranked, 1):
        r1 = dict1.get(team, {})
        r2 = dict2.get(team, {})
        score1 = r1.get('score', 0)
        score2 = r2.get('score', 0)
        diff = score2 - score1
        print(f"{i:<6}{team:<12}{score1:>10.2f}{score2:>12.2f}{diff:>9.2f}  "
              f"{r1.get('coverage', 0):>6.1f}{r2.get('coverage', 0):>8.1f}"
              f"{r1.get('dirt_eff', 0):>10.1f}{r2.get('dirt_eff', 0):>10.1f}"
              f"{r1.get('bat_cons', 0):>10.1f}{r2.get('bat_cons', 0):>10.1f}"
              f"{r1.get('movement', 0):>8.1f}{r2.get('movement', 0):>8.1f}")

    print("═" * 100)
    print("\nScoring weights:")
    for k, v in weights.items():
        print(f"  {k}: {v}")
    print("Bonuses:")
    for k, v in bonuses.items():
        print(f"  {k}: {v}")
    print("Thresholds:")
    for k, v in thresholds.items():
        print(f"  {k}: {v}")

if __name__ == '__main__':
    main()
