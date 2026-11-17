#!/usr/bin/env python3
"""boot_probe.py ELF OUT SYMBOL...: copy ELF with each SYMBOL's first instruction replaced by
`mtspr DBAT3U, r0`. Dolphin logs every BAT write ("DBAT updated 542"), even before OSInit sets
up any output, so a direct boot (Dolphin.exe -e) shows whether execution reached the symbol.
The function itself is broken by the probe: only "reached, nothing bad yet" results count.
Used to find where the pure-source image stopped (research/notes/pure-link.md)."""
import sys,struct; sys.path.insert(0,'tools')
from formats import Elf32
data=bytearray(open(sys.argv[1],'rb').read()); e=Elf32(bytes(data))
syms={x['name']:x for x in e.symbols() if x['type']==2}
rep=syms['OSReport']['address']
for n in sys.argv[3:]:
    s=syms[n]; sec=e.sections[s['section']]; off=sec['offset']+s['address']-sec['address']
    d=(rep-s['address'])&0x3fffffc
    data[off:off+4]=struct.pack(">I",0x7C1E83A6); print(n,hex(s['address']))
open(sys.argv[2],'wb').write(data)
