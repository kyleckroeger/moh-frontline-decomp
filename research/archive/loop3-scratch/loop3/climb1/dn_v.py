B=open('dn_block.txt').read()
V={}
V['ndot']=[(B,B.replace('world.vectors[2].Dot(n)','n.Dot(world.vectors[2])').replace('world.vectors[1].Dot(n)','n.Dot(world.vectors[1])').replace('world.vectors[0].Dot(n)','n.Dot(world.vectors[0])'))]
V['ndot2']=[(B,B.replace('world.vectors[2].Dot(n)','n.Dot(world.vectors[2])'))]
V['ndot1']=[(B,B.replace('world.vectors[1].Dot(n)','n.Dot(world.vectors[1])'))]
V['ndot0']=[(B,B.replace('world.vectors[0].Dot(n)','n.Dot(world.vectors[0])'))]
V['ndot12']=[(B,B.replace('world.vectors[2].Dot(n)','n.Dot(world.vectors[2])').replace('world.vectors[1].Dot(n)','n.Dot(world.vectors[1])'))]
V['ndot01']=[(B,B.replace('world.vectors[0].Dot(n)','n.Dot(world.vectors[0])').replace('world.vectors[1].Dot(n)','n.Dot(world.vectors[1])'))]
V['ndot02']=[(B,B.replace('world.vectors[0].Dot(n)','n.Dot(world.vectors[0])').replace('world.vectors[2].Dot(n)','n.Dot(world.vectors[2])'))]
V['noref']=[(B,B.replace('const CVector3& n = plane->m_normal;\n','').replace('(n)','(plane->m_normal)'))]
