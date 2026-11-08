"""poolvals.py <unit id>: print the original bytes (and float value) of each pool reference in a unit manifest (research helper)."""
import json, subprocess, struct, sys
import fast
d = json.load(open(f'config/GMFE69/{sys.argv[1]}.json'))
for r in d.get('pool_references', []):
    a = int(r['address'], 16)
    out = subprocess.run([fast.OBJDUMP, '-s', '--start-address=' + hex(a), '--stop-address=' + hex(a + r['size']), fast.ELF], capture_output=True, text=True).stdout.splitlines()
    data = ''.join(l.split()[1:1 + (r['size'] + 3) // 4] and ''.join(l[10:45].split()) for l in out if l.startswith(' '))[:r['size'] * 2]
    val = struct.unpack('>f', bytes.fromhex(data))[0] if r['size'] == 4 else (struct.unpack('>d', bytes.fromhex(data))[0] if r['size'] == 8 else bytes.fromhex(data))
    print(r['section'], r['offset'], hex(a), data, val)
