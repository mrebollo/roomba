#!/usr/bin/env python3
"""
Analyze 24-25 CSV results with new scoring system
"""

import csv
import sys
from collections import defaultdict
import math

def calculate_new_scores(data):
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
        team = row['id']
        
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
        
        # Weighted score
        total_score = (
            avg_coverage * 0.30 +
            avg_dirt_eff * 0.35 +
            avg_bat_cons * 0.20 +
            avg_movement * 0.15
        )
        
        # Bonuses
        if stats['crashes'] == 0:
            total_score += 5  # Completion bonus
        else:
            total_score -= stats['crashes'] * 10  # Crash penalty
        
        # Low bumps bonus (< 10%)
        if avg_movement > 90:  # Equivalent to < 10% bumps
            total_score += 3
        
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
        team = row['id']
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
    if len(sys.argv) < 2:
        print("Usage: python3 analyze_csv.py <csv_file>")
        sys.exit(1)
    
    # Read CSV
    with open(sys.argv[1], 'r', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        data = list(reader)
    
    print(f"Loaded {len(data)} rows from CSV\n")
    
    # Calculate new scores
    new_results = calculate_new_scores(data)
    
    # Calculate old scores
    old_results = calculate_old_scores(data)
    old_dict = {r['team']: r['old_score'] for r in old_results}
    
    # Display comparison
    print("═" * 100)
    print("COMPARISON: Old System vs New System".center(100))
    print("═" * 100)
    print(f"{'Rank':<6}{'Team':<12}{'Old Score':<12}{'New Score':<12}{'Diff':<10}{'Cover':<8}{'DirtEff':<10}{'BatCons':<10}{'Movement':<10}")
    print("─" * 100)
    
    for i, result in enumerate(new_results[:15], 1):  # Top 15
        team = result['team']
        old_score = old_dict.get(team, 0)
        diff = result['score'] - old_score
        
        print(f"{i:<6}{team:<12}{old_score:>10.2f}{result['score']:>12.2f}{diff:>9.2f}  "
              f"{result['coverage']:>6.1f}{result['dirt_eff']:>10.1f}{result['bat_cons']:>10.1f}{result['movement']:>10.1f}")
    
    print("═" * 100)
    print(f"\nScoring weights: Coverage=30% DirtEff=35% BatCons=20% Movement=15%")
    print(f"Bonuses: Completion=+5, Low Bumps=+3, Crash Penalty=-10 each\n")

if __name__ == '__main__':
    main()
