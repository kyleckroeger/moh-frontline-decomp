# mk2.sh name middlefile : H + middle + T
D=/home/kyle/workspace/moh-frontline/scratch/loop3/struct6
cat $D/tlt/H.txt $2 $D/tlt/T.txt | sh $D/mk.sh $1 $3 | tr '\n' ' '; echo
