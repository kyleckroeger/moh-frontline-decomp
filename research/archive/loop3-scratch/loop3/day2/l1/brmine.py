# brchk + score restricted to late1.tsv rows
import csv,sys,runpy,io,contextlib
sys.path.insert(0,'research/tools')
mine={r['function'] for r in csv.DictReader(open('scratch/loop3/day2/late1.tsv'),delimiter='\t')}
src=open('research/tools/brchk.py').read()
src=src.replace("    if rl>mx: continue","    if rl>mx or r['function'] not in mine: continue")
src=src.replace("for r in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\\t'):","mine=MINE\nfor r in csv.DictReader(open('research/drafts/index.tsv'),delimiter='\\t'):")
sys.argv=['brchk','100000']
exec(compile(src.replace('MINE',repr(mine)),'brchk','exec'),{'__name__':'__main__'})
