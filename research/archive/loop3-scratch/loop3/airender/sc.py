"""sc.py tmpl src func addr size: differing lines (score.real)."""
import sys
sys.path.insert(0, '/home/kyle/workspace/moh-frontline/research/tools')
import score
t, s, f, a, n = sys.argv[1:6]
print(score.real(t, open(s, errors='replace').read(), f, a, n))
