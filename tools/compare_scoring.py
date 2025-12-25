
import csv
import math
import statistics

STATS_FILE = 'competition/stats.csv'

# Config weights (defaults from libscore.c)
W_COVERAGE = 0.30
W_DIRT = 0.35
W_BATTERY = 0.20
W_QUALITY = 0.15

# Scaling factors
DIRT_SCALE = 100.0
BATTERY_SCALE = 1000.0

def load_stats():
    teams = {}
    try:
        with open(STATS_FILE, 'r') as f:
            reader = csv.DictReader(f)
            for row in reader:
                team = row['team']
                if team not in teams:
                    teams[team] = []
                
                # Parse metrics
                cell_total = int(row['cell_total'])
                cell_visited = int(row['cell_visited'])
                dirt_cleaned = int(row['dirt_cleaned'])
                bat_total = float(row['bat_total'])
                bat_mean = float(row['bat_mean'])
                bumps = int(row['bumps'])
                forward = int(row['forward'])
                turn = int(row['turn'])
                
                # Calculate basic scores per map
                coverage = (cell_visited / cell_total * 100) if cell_total > 0 else 0
                coverage = min(100, coverage)
                
                dirt_eff = (dirt_cleaned / bat_total * DIRT_SCALE) if bat_total > 0 else 0
                
                bat_cons = (bat_mean / BATTERY_SCALE * 100)
                bat_cons = min(100, bat_cons)
                
                total_moves = forward + turn + bumps
                quality = (1.0 - (bumps / total_moves)) * 100 if total_moves > 0 else 0
                quality = max(0, quality)
                
                teams[team].append({
                    'coverage': coverage,
                    'dirt_eff': dirt_eff,
                    'bat_cons': bat_cons,
                    'quality': quality
                })
    except FileNotFoundError:
        print(f"Error: {STATS_FILE} not found.")
        return None
    return teams

def calculate_consistency(values):
    if len(values) < 2: return 100.0
    stdev = statistics.stdev(values)
    # Formula from libscore: 100 / (1 + stdev_avg) -> This maps [0, inf) to [100, 0)
    # But user mentioned "100 - std_deviation" in conf comments.
    # Let's show both slightly. Libscore logic:
    return 100.0 / (1.0 + stdev) if stdev >= 0 else 100.0

def analyze(teams):
    print(f"{'TEAM':<15} | {'CURRENT':<8} | {'WEIGHTED':<8} | {'BONUS':<8} | {'CONSIST':<8}")
    print("-" * 75)
    
    results = []
    
    for team, maps in teams.items():
        # Averages
        avg_cov = statistics.mean(m['coverage'] for m in maps)
        avg_dirt = statistics.mean(m['dirt_eff'] for m in maps)
        avg_bat = statistics.mean(m['bat_cons'] for m in maps)
        avg_qual = statistics.mean(m['quality'] for m in maps)
        
        # Current Score Formula
        current_score = (avg_cov * W_COVERAGE + 
                         avg_dirt * W_DIRT + 
                         avg_bat * W_BATTERY + 
                         avg_qual * W_QUALITY)
        
        # Consistency Calculation
        stdev_cov = statistics.stdev([m['coverage'] for m in maps]) if len(maps) > 1 else 0
        stdev_dirt = statistics.stdev([m['dirt_eff'] for m in maps]) if len(maps) > 1 else 0
        stdev_bat = statistics.stdev([m['bat_cons'] for m in maps]) if len(maps) > 1 else 0
        stdev_qual = statistics.stdev([m['quality'] for m in maps]) if len(maps) > 1 else 0
        
        avg_stdev = (stdev_cov + stdev_dirt + stdev_bat + stdev_qual) / 4.0
        consistency = 100.0 / (1.0 + avg_stdev)
        
        # Strategy 1: Weighted (90% Current + 10% Consistency)
        weighted_score = (current_score * 0.9) + (consistency * 0.1)
        
        # Strategy 2: Fixed Bonus (+5 if Consistency > 40)
        # Using 40 as threshold for "decent stability"
        bonus_score = current_score
        if consistency > 40:
            bonus_score += 5.0
        
        results.append((team, current_score, weighted_score, bonus_score, consistency))

    # Sort by Current Score
    results.sort(key=lambda x: x[1], reverse=True)
    
    for r in results:
        # Show scores
        print(f"{r[0]:<15} | {r[1]:6.2f}   | {r[2]:6.2f}   | {r[3]:6.2f}   | {r[4]:6.2f}")

if __name__ == "__main__":
    data = load_stats()
    if data:
        analyze(data)
