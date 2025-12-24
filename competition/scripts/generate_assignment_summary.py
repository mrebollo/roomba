#!/usr/bin/env python3
"""Generate assignment summary using inverse method and produce CSV/report.

This is the same script previously in `tools/` but relocated to
`competition/scripts/`. Adjusted `ROOT` resolution so paths remain
relative to the repository root.
"""
import csv
import os
import re
import unicodedata
import shutil
import hashlib
from pathlib import Path
from collections import defaultdict

# When located at repo/competition/scripts, parents[2] is repo root
ROOT = Path(__file__).resolve().parents[2]
ENTREGAS_DIR = ROOT / 'competition' / 'entregas'
TEAMS_DIR = ROOT / 'competition' / 'teams'
TEAMS_DIR.mkdir(parents=True, exist_ok=True)

def norm(s):
    if not s:
        return ''
    s = s.lower()
    s = unicodedata.normalize('NFD', s)
    s = ''.join(ch for ch in s if unicodedata.category(ch) != 'Mn')
    s = re.sub(r"[^a-z0-9 ]+", ' ', s)
    s = re.sub(r'\s+', ' ', s).strip()
    return s

# find CSV with teams (same heuristic as other scripts)
csv_candidates = list(ENTREGAS_DIR.glob('*.csv'))
if not csv_candidates:
    print('No CSV found in', ENTREGAS_DIR)
    raise SystemExit(1)
CSV_PATH = csv_candidates[0]

# load teams
teams = []
with open(CSV_PATH, newline='') as f:
    data = f.read()
if data.startswith('```'):
    data = '\n'.join(data.splitlines()[1:])
reader = csv.reader(data.splitlines())
rows = list(reader)
start_idx = 0
for i, r in enumerate(rows):
    if any('team' in (c or '').lower() for c in r):
        start_idx = i
        break
rows = rows[start_idx:]
for r in rows:
    if not r or len(r) < 2:
        continue
    teamnum = r[0].strip()
    if not teamnum.isdigit():
        continue
    teamnum = int(teamnum)
    members = []
    for c in r[5:8]:
        if c and c.strip():
            members.append(c.strip())
    teamname = r[4].strip() if len(r) > 4 else ''
    teams.append({'team': teamnum, 'teamname': teamname, 'members': members})

# collect entregas
entrega_folders = [p for p in ENTREGAS_DIR.iterdir() if p.is_dir()]
entregas = []
for ef in entrega_folders:
    files_c = list(ef.rglob('*.c'))
    htmls = list(ef.rglob('*.html')) + list(ef.rglob('*.htm'))
    html_text = ''
    for h in htmls:
        try:
            html_text += ' ' + h.read_text(errors='ignore')
        except Exception:
            pass
    display_name = ef.name
    name_no_paren = re.sub(r"\(.*?\)", '', display_name).strip()
    possible_names = [name_no_paren]
    if ',' in name_no_paren:
        parts = name_no_paren.split(',', 1)
        reordered = (parts[1].strip() + ' ' + parts[0].strip()).strip()
        if reordered and reordered not in possible_names:
            possible_names.append(reordered)
    norm_names = [norm(x) for x in possible_names if x]
    entregas.append({'folder': ef, 'c_files': files_c, 'html_text': html_text, 'name': display_name, 'possible_names': possible_names, 'norm_names': norm_names})

# build folder -> candidate teams mapping
folder_candidates = defaultdict(list)
team_candidate_folders = defaultdict(list)
team_tokens = {t['team']: [norm(m) for m in t['members']] for t in teams}
for ent in entregas:
    for t in teams:
        toks = team_tokens.get(t['team'], [])
        matched = False
        for tok in toks:
            if not tok:
                continue
            if any(tok in nn for nn in ent.get('norm_names', [])):
                folder_candidates[ent['folder'].name].append((t['team'], 'folder_name'))
                team_candidate_folders[t['team']].append(ent['folder'].name)
                matched = True
                break
        if matched:
            continue
        text = norm(ent.get('html_text', ''))
        for tok in toks:
            if not tok:
                continue
            if tok in text:
                folder_candidates[ent['folder'].name].append((t['team'], 'html_text'))
                team_candidate_folders[t['team']].append(ent['folder'].name)
                break

# prepare summary mapping for every entrega folder
summary_map = {}

# helper to copy main.c if present and team dir doesn't already have main.c
def copy_main_to_team(teamnum, entrega_folder_name, ent):
    teamdir = TEAMS_DIR / f'team{teamnum:02d}'
    teamdir.mkdir(parents=True, exist_ok=True)
    dst = teamdir / 'main.c'
    if dst.exists():
        return True, dst
    # choose main candidate or first .c
    cands = ent.get('c_files', [])
    if not cands:
        return False, None
    mainc = None
    for p in cands:
        if 'main' in p.name.lower():
            mainc = p
            break
    if not mainc:
        mainc = cands[0]
    try:
        shutil.copy(str(mainc), str(dst))
        return True, dst
    except Exception:
        return False, dst

# initialize summary_map with defaults for each entrega
for e in entregas:
    name = e['folder'].name
    has_c = bool(e.get('c_files'))
    # default: no team assigned
    summary_map[name] = {'team_numbers': [], 'team_names': [], 'team_members': [], 'action': ('no_file' if not has_c else '')}

# Process teams: assign where unique candidate or identical duplicates
for t in teams:
    candidates = list(dict.fromkeys(team_candidate_folders.get(t['team'], [])))
    print(f"Procesando equipo: team{t['team']:02d} - {t['teamname']}")
    if len(candidates) == 1:
        assigned = candidates[0]
        ent = next(e for e in entregas if e['folder'].name == assigned)
        copied, dst = copy_main_to_team(t['team'], assigned, ent)
        action = 'copied' if copied else ('no_file' if not ent.get('c_files') else 'copied')
        print(f"{assigned} -> team{t['team']:02d} : {action}")
        summary_map[assigned]['team_numbers'].append(f"team{t['team']:02d}")
        summary_map[assigned]['team_names'].append(t['teamname'])
        summary_map[assigned]['team_members'].append(';'.join(t['members']))
        # only set action to copied if it wasn't a duplicate/conflict marker
        if summary_map[assigned]['action'] == '' or summary_map[assigned]['action'] == '':
            summary_map[assigned]['action'] = action
    elif len(candidates) > 1:
        # compare main.c hashes
        paths = []
        for cand in candidates:
            ent = next(e for e in entregas if e['folder'].name == cand)
            cands = ent.get('c_files', [])
            if not cands:
                continue
            mainc = None
            for p in cands:
                if 'main' in p.name.lower():
                    mainc = p
                    break
            if not mainc:
                mainc = cands[0]
            paths.append((cand, str(mainc)))
        hashes = {}
        for cand, path in paths:
            try:
                with open(path, 'rb') as fh:
                    h = hashlib.sha256(fh.read()).hexdigest()
            except Exception:
                h = None
            hashes[cand] = h
        uniq = set(h for h in hashes.values() if h)
        if len(uniq) == 1 and len(uniq) > 0:
            # identical: auto-assign first, but mark all candidate folders as duplicated
            assigned = candidates[0]
            ent = next(e for e in entregas if e['folder'].name == assigned)
            copied, dst = copy_main_to_team(t['team'], assigned, ent)
            disp = ('copied' if copied else 'no_file')
            print(f"{assigned} -> team{t['team']:02d} : duplicated (identical main.c) -> {disp}")
            for cand in candidates:
                summary_map[cand]['team_numbers'].append(f"team{t['team']:02d}")
                summary_map[cand]['team_names'].append(t['teamname'])
                summary_map[cand]['team_members'].append(';'.join(t['members']))
                # mark as duplicated for all candidate folders
                summary_map[cand]['action'] = 'duplicated'
        else:
            # conflict: mark all candidate folders as conflict for manual review
            for cand in candidates:
                print(f"{cand} -> team{t['team']:02d} : conflict (different files) ")
                summary_map[cand]['team_numbers'].append(f"team{t['team']:02d}")
                summary_map[cand]['team_names'].append(t['teamname'])
                summary_map[cand]['team_members'].append(';'.join(t['members']))
                # mark as conflict to indicate manual attention needed
                summary_map[cand]['action'] = 'conflict'

# Now handle entregas with no candidate teams: create new sequential teams and copy main.c
all_entrega_names = sorted([e['folder'].name for e in entregas])
unmatched = [n for n in all_entrega_names if not folder_candidates.get(n)]
existing_team_dirs = [p.name for p in TEAMS_DIR.iterdir() if p.is_dir() and p.name.startswith('team')]
nums = [int(re.sub(r'[^0-9]', '', n)) for n in existing_team_dirs if re.sub(r'[^0-9]', '', n)]
start_num = max(nums) if nums else 0
for name in unmatched:
    start_num += 1
    teamnum = start_num
    teamname = f'team{teamnum:02d}'
    ent = next(e for e in entregas if e['folder'].name == name)
    copied, dst = copy_main_to_team(teamnum, name, ent)
    action = 'generated' if copied else 'no_file'
    print(f"{name} -> {teamname} : {action}")
    summary_map[name]['team_numbers'].append(teamname)
    summary_map[name]['team_names'].append(teamname)
    summary_map[name]['team_members'].append('')
    summary_map[name]['action'] = action

# write CSV with all folders ordered by folder name
out = ROOT / 'competition' / 'assignment_summary.csv'
with open(out, 'w', newline='') as f:
    w = csv.writer(f)
    w.writerow(['entrega_folder', 'team_number', 'team_name', 'team_members', 'action'])
    for name in sorted(summary_map.keys(), key=lambda s: s.lower()):
        info = summary_map[name]
        team_numbers = ';'.join(info['team_numbers'])
        team_names = ';'.join([n for n in info['team_names'] if n])
        team_members = ';'.join([m for m in info['team_members'] if m])
        action = info['action'] if info['action'] else ('copied' if info['team_numbers'] and info['team_members'] else 'no_file')
        w.writerow([name, team_numbers, team_names, team_members, action])

print('\nWrote', out)
