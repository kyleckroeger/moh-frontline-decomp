"""multi.py <settings> <file> name:addr:size ... : one compile, score several functions."""
import sys,json,tempfile
sys.path.insert(0,'research/tools'); sys.path.insert(0,'scratch/game')
import score,fast,varmap
def run(t,text,specs):
    with tempfile.TemporaryDirectory() as w:
        obj,log=fast.compile_object(json.load(open(t)),text,w)
        if not obj: return None
        out=[]
        for f,a,n in specs:
            e=hex(int(a,16)+int(n))
            m=varmap.rows(['--disassemble='+f,obj]); o=varmap.rows(['--start-address=0x'+a,'--stop-address='+e,fast.ELF])
            out.append(sum(1 for x,y in zip(o,m) if not score.eq(x,y))+abs(len(o)-len(m)) if m else None)
        return out
if __name__=='__main__':
    specs=[x.split(':') for x in sys.argv[3:]]
    print(run(sys.argv[1],open(sys.argv[2]).read(),specs))
