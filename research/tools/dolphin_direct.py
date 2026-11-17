#!/usr/bin/env python3
"""dolphin_direct.py ELF SECS: boot an ELF directly with Dolphin.exe -e (Dolphin then knows its
symbols and its MI log names invalid MMIO writes); leaves user/Logs/dolphin.log in the work dir.
Used with boot_probe.py and entry_trace.py (research/notes/pure-link.md)."""
# direct.py ELF SECS: boot ELF directly in Dolphin (Dolphin then knows its symbols); leave user/Logs/dolphin.log
import sys, json, subprocess, os, shutil
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__))))
import dolphin_boot as d
W='/mnt/c/Users/kylec/AppData/Local/Temp/mohf-dolphin'
shutil.copy(sys.argv[1], os.path.join(W,'direct.elf'))
cfg=json.load(open(os.path.join(W,'dolphin_boot.json')))
log=os.path.join(W,'user','Logs','dolphin.log')
subprocess.run(["/mnt/c/Windows/System32/taskkill.exe","/IM","Dolphin.exe","/F"],capture_output=True)
if os.path.exists(log): os.remove(log)
cmd=[d.POWERSHELL,"-NoProfile","-ExecutionPolicy","Bypass","-File",d.winpath(os.path.join(W,"run.ps1")),"-exe",cfg["dolphin"],"-iso",d.winpath(os.path.join(W,"direct.elf")),"-user",d.winpath(os.path.join(W,"user")),"-secs",sys.argv[2],"-out",d.winpath(os.path.join(W,"direct")),"-shot","60"]
subprocess.run(cmd,capture_output=True,text=True,timeout=int(sys.argv[2])+120)
