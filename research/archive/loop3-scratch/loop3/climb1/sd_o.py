BLOCKFILE='sd_block.txt'; exec(open('gspec.py').read())
# member-call order: SetAlphaOverride must precede its m_alpha store
import re
for i,s in enumerate(S):
    if s.startswith('m_modelMaterial.m_alpha'): DEP[i].append(S.index('m_modelMaterial.SetAlphaOverride();'))
    if s.startswith('m_headMaterial.m_alpha'): DEP[i].append(S.index('m_headMaterial.SetAlphaOverride();'))
    if s.startswith('if (t'): DEP[i].append(S.index('t = (m_fadeTime - 0.5f * m_fadeDuration) / (0.5f * m_fadeDuration);'))
    if 'alpha = 1.0f - t' in s: DEP[i].append(S.index('if (t >= 1.0f) t = 1.0f;'))
