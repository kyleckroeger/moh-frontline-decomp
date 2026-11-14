# sus.sh draft cand: lines added in cand that look like declined forms
d=$1; c=$2
echo "== $c ($(diff $d $c | grep -c '^>') added lines)"
diff $d $c | grep '^>' | grep -nE '[a-zA-Z_\]\)]\s*=\s*[a-zA-Z_][a-zA-Z_0-9.>\-]*\s*=[^=]|\[1\];|Copy\(|RetV|Identity|operator\[\]|pad|unk[0-9a-f]*\[|& *\w+ *= *\w+;|\* *const +\w+ *= *&|\(char ?\*\) ?\w+ ?\+|pragma|asm|volatile|vtable|__vt' | head -20
