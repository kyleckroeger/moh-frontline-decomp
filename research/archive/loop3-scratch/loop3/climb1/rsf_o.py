BLOCK='''        float* const last = in + ((state->position >> 16) - state->lead);
        state->history[0] = last[0];
        state->history[1] = last[1];
        state->history[2] = last[2];
        state->history[3] = last[3];
        state->position &= 0xffffLL;
'''
IND='        '
S=[l.strip() for l in BLOCK.strip('\n').split('\n')]
DEP={0:[],1:[0],2:[0],3:[0],4:[0],5:[0]}
EXT='.c'
