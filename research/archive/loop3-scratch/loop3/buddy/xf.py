"""xf.py <draft> <out> <form>: swap in the nested BuddyState BuddyInfo definition with a dead block read form."""
import re,sys
src=open(sys.argv[1]).read(); form=sys.argv[3]
import os
OPEQ='    BuddyState& operator=(const BuddyState& o) {\n        path = o.path;\n        moving = o.moving;\n        active = o.active;\n        return *this;\n    }\n' if os.environ.get('OPEQ') else ''
READ={'self':('', ', state(state)'), 'check':('Check(state); ',''), 'get':('GetState(); ',''), 'unused':('BuddyState unused = state; ',''),'none':('','')}[form]
pre = "inline void Check(BuddyState) {}\n" if form=='check' else ''
get = "    BuddyState GetState() const { return state; }\n" if form=='get' else ''
defn=f"""struct BuddyState {{
    int path;
    int moving;
    bool active;
{OPEQ}}};
{pre}
struct BuddyInfo {{
{get}    BuddyInfo() : soldier(0), progress(0){READ[1]} {{
        {READ[0]}state.moving = 0;
        state.active = false;
        state.path = 0;
    }}
    BuddyInfo(const BuddyInfo& other) : soldier(other.soldier), progress(other.progress) {{
        state.moving = other.state.moving;
        state.active = other.state.active;
        state.path = other.state.path;
    }}

    CBuddySoldier* soldier;
    int progress;
    BuddyState state;
}};
"""
# cut from 'struct BuddyState {' (if any) or 'struct BuddyInfo {' to end of BuddyInfo
start = src.find('struct BuddyState {')
if start<0: start=src.find('struct BuddyInfo {')
bi=src.find('struct BuddyInfo {')
end=src.find('\n};',bi)+4
body=src[:start]+defn+src[end:]
head,tail=body[:start+len(defn)],body[start+len(defn):]
tail=re.sub(r'\b(buddy|b|it|info|\])(\.|->)(path|moving|active)\b', r'\1\2state.\3', tail)
tail=tail.replace('state.state.','state.')
open(sys.argv[2],'w').write(head+tail)
