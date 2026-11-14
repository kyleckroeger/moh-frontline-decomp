import sys,itertools
s=open('ss.cpp').read()
a=s.index('        if (record->type == 0) {\n')
b=s.index('    SNDSYS_entercritical();')
body_old=s[a:b]
speed=body_old[body_old.index('            CStaticObject* object'):body_old.index('\n        } else {\n')+1]
REC='SoundScheduleRegistrationRecord* record'
def az_if(mp_direct, ret):
    mp = ('return GetAzimuth(nodePosition, g_scene.FindClosestPlayer(nodePosition));' if mp_direct else 'return GetClosestAzimuth(nodePosition);') if ret else \
         ('azimuth = GetAzimuth(nodePosition, g_scene.FindClosestPlayer(nodePosition));' if mp_direct else 'azimuth = GetClosestAzimuth(nodePosition);')
    if ret:
        return f'''static inline int NodeAzimuth({REC}) {{
    CVector3 position;
    record->node->GetPosition(position);
    if (g_bInMultiplayerMode) {{
        CVector3 nodePosition;
        record->node->GetPosition(nodePosition);
        {mp}
    }}
    return GetAzimuth(position, g_scene.GetPlayer(0));
}}
'''
    return f'''static inline int NodeAzimuth({REC}) {{
    CVector3 position;
    record->node->GetPosition(position);
    int azimuth;
    if (g_bInMultiplayerMode) {{
        CVector3 nodePosition;
        record->node->GetPosition(nodePosition);
        {mp}
    }} else {{
        azimuth = GetAzimuth(position, g_scene.GetPlayer(0));
    }}
    return azimuth;
}}
'''
def dist_if(gnp):
    if gnp=='inline': mp='g_scene.FindClosestPlayer(GetNodePosition(record->node))->GetPosition(listener);'
    else: mp='{\n        CVector3 nodePosition;\n        record->node->GetPosition(nodePosition);\n        g_scene.FindClosestPlayer(nodePosition)->GetPosition(listener);\n    }'
    return f'''static inline float NodeDistance({REC}) {{
    CVector3 delta;
    record->node->GetPosition(delta);
    CVector3 listener;
    if (g_bInMultiplayerMode)
        {mp}
    else
        g_scene.GetPlayer(0)->GetPosition(listener);
    delta.d.v[0] -= listener.d.v[0];
    delta.d.v[1] -= listener.d.v[1];
    delta.d.v[2] -= listener.d.v[2];
    return sqrtf(delta.d.v[0] * delta.d.v[0] + delta.d.v[1] * delta.d.v[1] + delta.d.v[2] * delta.d.v[2]);
}}
'''
def az_else(param):
    return f'''static inline int PositionAzimuth({param} position) {{
    if (g_bInMultiplayerMode)
        return GetAzimuth(position, g_scene.FindClosestPlayer(position));
    return GetAzimuth(position, g_scene.GetPlayer(0));
}}
'''
def dist_else(param):
    return f'''static inline float PositionDistance({param} position) {{
    CVector3 listener;
    if (g_bInMultiplayerMode)
        g_scene.FindClosestPlayer(position)->GetPosition(listener);
    else
        g_scene.GetPlayer(0)->GetPosition(listener);
    listener.d.v[0] -= position.d.v[0];
    listener.d.v[1] -= position.d.v[1];
    listener.d.v[2] -= position.d.v[2];
    return sqrtf(listener.d.v[0] * listener.d.v[0] + listener.d.v[1] * listener.d.v[1] + listener.d.v[2] * listener.d.v[2]);
}}
'''
body_new=f'''        if (record->type == 0) {{
            record->aemsEvent[record->azimuthParam] = NodeAzimuth(record);
            float distance = NodeDistance(record);
            record->aemsEvent[record->distanceParam] = (int)(256.0f * (record->distanceScale * distance));
{speed}        }} else {{
            record->aemsEvent[record->azimuthParam] = PositionAzimuth(record->position);
            float distance = PositionDistance(record->position);
            record->aemsEvent[record->distanceParam] = (int)(256.0f * (record->distanceScale * distance));
        }}
    }}
'''
anchor='void SoundSchedule::Update(int frames) {'
for mpd,ret,gnp,par in itertools.product([0,1],[0,1],['inline','local'],['const CVector3&']):
    pre=az_if(mpd,ret)+dist_if(gnp)+az_else(par)+dist_else(par)
    t=s.replace(body_old,body_new).replace(anchor,pre+anchor)
    open(f'g/ss_{mpd}{ret}{gnp[0]}.cpp','w').write(t)
