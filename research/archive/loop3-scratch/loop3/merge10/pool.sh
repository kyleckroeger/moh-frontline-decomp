#!/bin/sh
# pool.sh TPL SRC IDX : compare compiled .sdata2/.rodata/.data bytes with the original file record's
cd /home/kyle/workspace/moh-frontline; M=scratch/loop3/merge10
O=$(mktemp -d /tmp/claude-1000/pl.XXXX); (ulimit -v 3000000; python3 $M/chk.py $1 $2 > $O/c 2>&1); OB=$(grep ^OBJ $O/c|cut -d' ' -f2)
for s in .sdata2 .rodata .data .sdata; do
  sz=$(build/tools/powerpc-eabi-objdump -h $OB | awk -v s=$s '$2==s{print $3}'); [ -z "$sz" ] && continue
  a=$(sed -n "$(($3+4)),$(($3+30))p" scratch/loop3/merge1/orig_syms.txt | awk -v s=$s '$3=="d" && $4==s {print $1; exit}')
  [ -z "$a" ] && { echo "$s: no original section"; continue; }
  x=$(build/tools/powerpc-eabi-objdump -s -j $s $OB | tail -n +5 | cut -c7-41 | tr -d ' \n')
  y=$(build/tools/powerpc-eabi-objdump -s -j $s --start-address=0x$a --stop-address=$(printf '0x%x' $((0x$a+0x$sz))) orig/GMFE69/Moh2RelGC.elf | tail -n +5 | cut -c11-45 | tr -d ' \n')
  [ "$x" = "$y" ] && echo "$s same ($sz)" || { echo "$s DIFF"; echo " c $x"; echo " o $y"; }
done
rm -rf $O
