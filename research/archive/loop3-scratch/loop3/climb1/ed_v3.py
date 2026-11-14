B=open('edb_block.txt').read()
V={}
V['dataloc']=[(B,'''    CFont* renderFont = font->font;
    CFontData* data = renderFont->m_data;
    stringPacket.color = data->GetColor();
    stringPacket.scaleX = data->scaleX;
    stringPacket.scaleY = data->scaleY;
''')]
