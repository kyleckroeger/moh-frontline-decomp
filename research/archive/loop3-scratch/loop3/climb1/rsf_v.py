V={}
V['shl']=[('(pitch & 0xffff) << 16','pitch << 16')]
V['posmask']=[('        state->position = (whole << 16) | (fraction >> 16);','        state->position = (fraction >> 16) | (whole << 16);')]
