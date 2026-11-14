#!/bin/sh
# usage: runv.sh VARIANTDIR TAG
cd /home/kyle/workspace/moh-frontline
ulimit -v 4000000
start=$(date +%s)
TRY_HEADER_DIR=$PWD/scratch/loop3/cv1/$1 TRY_HEADER_WORK=$PWD/scratch/loop3/cv1/w_$2 timeout 7200 python3 scratch/loop3/cv1/drv.py 4 $(cat scratch/loop3/cv1/own.txt scratch/loop3/cv1/hdr.txt) > scratch/loop3/cv1/res_$2.tsv 2>scratch/loop3/cv1/err_$2.log
echo "done $(( $(date +%s)-start ))s" >> scratch/loop3/cv1/res_$2.tsv
