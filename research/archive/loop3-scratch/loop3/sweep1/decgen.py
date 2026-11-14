import sys,re
base=open('dec0.cpp').read()
ctor_old='''DECODER::DECODER(const CODEC_IDATA* idata) {
    m_idata = *idata;
    m_codec = 0;
    m_pending = 0;
}'''
assert ctor_old in base
V={}
V['cc_init']=( 'CODEC_IDATA(const CODEC_IDATA& o) { Clear(); *this = o; }\n    CODEC_IDATA& operator=(const CODEC_IDATA& o) { m_streamer=o.m_streamer; m_getChunk=o.m_getChunk; m_releaseChunk=o.m_releaseChunk; m_value=o.m_value; return *this; }',
 '''DECODER::DECODER(const CODEC_IDATA* idata) : m_idata(*idata) {
    m_codec = 0;
    m_pending = 0;
}''')
V['constnone']=(None,'''DECODER::DECODER(const CODEC_IDATA* idata) {
    CODEC* const none = 0;
    m_idata = *idata;
    m_codec = none;
    m_pending = (CHUNK*)none;
}''')
V['late']=(None,'''DECODER::DECODER(const CODEC_IDATA* idata) {
    const CODEC_IDATA* src;
    DECODER* const self = this;
    src = idata;
    self->m_idata = *src;
    m_codec = 0;
    m_pending = 0;
}''')
V['selfconst']=(None,'''DECODER::DECODER(const CODEC_IDATA* idata) {
    const CODEC_IDATA* const src = idata;
    m_idata = *src;
    m_codec = 0;
    m_pending = 0;
}''')
V['chain']=(None,'''DECODER::DECODER(const CODEC_IDATA* idata) {
    m_idata = *idata;
    m_pending = (CHUNK*)(m_codec = 0);
}''')
V['helper']=(None,'''static inline void InitDecoder(DECODER* d, const CODEC_IDATA* idata) {
    d->m_idata = *idata;
    d->m_codec = 0;
    d->m_pending = 0;
}
DECODER::DECODER(const CODEC_IDATA* idata) {
    InitDecoder(this, idata);
}''')
V['memberinline']=('inline void Set(const CODEC_IDATA* p) { *this = *p; }','''DECODER::DECODER(const CODEC_IDATA* idata) {
    m_idata.Set(idata);
    m_codec = 0;
    m_pending = 0;
}''')
V['clearnested']=(None,'''DECODER::DECODER(const CODEC_IDATA* idata) {
    m_idata = *idata;
    m_codec = 0;
    m_pending = 0;
    m_idata.m_value = m_idata.m_value;
}''')
for k,(cls,ctor) in V.items():
    t=base.replace(ctor_old,ctor)
    if cls: t=t.replace('    CODEC_IDATA(STREAMER*, GETCHUNKFUNC','    '+cls+'\n    CODEC_IDATA(STREAMER*, GETCHUNKFUNC')
    open(f'dv_{k}.cpp','w').write(t)
