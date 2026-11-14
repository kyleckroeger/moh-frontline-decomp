#!/bin/sh
# usage: rsbl.sh <exact RS symbol> [-d]  -> list bl targets (or full disasm with -d)
cd /home/kyle/workspace/moh-frontline
OD=build/tools/powerpc-eabi-objdump
ELF=../moh-rising-sun/orig/GR8E69/MOH3RDVD.ELF
line=$(awk -v n="$1" '$NF==n' scratch/loop3/rs1/rs_syms.txt | head -1)
[ -z "$line" ] && { echo "not found: $1"; exit 1; }
a=$(echo $line | awk '{print $1}'); sz=$(echo "$line" | awk '{print $(NF-1)}')
e=$(printf '%x' $((0x$a + 0x$sz)))
if [ "$2" = "-d" ]; then $OD -d --start-address=0x$a --stop-address=0x$e $ELF | tail -n +7; else
$OD -d --start-address=0x$a --stop-address=0x$e $ELF | grep -E "\sbl\s" | sed 's/.*<\(.*\)>.*/\1/' | uniq -c; fi
