#!/bin/sh
cd /home/kyle/workspace/moh-frontline
T=scratch/loop3/merge6
sh $T/land3.sh bulletfactory src/game/bulletfactory.cpp $T/tpl_bulletfactory.json bulletfactory.cpp 7891 "bulletfactory_sprites bulletfactory bulletfactory_init bulletfactory_thrown_ctor bulletfactory_thrown_pool bulletfactory_projectile_ctor bulletfactory_projectile_pool" "- 2026-11-13 [loop merge6] merged bulletfactory.cpp (7 fragments, 5800 B): one CBulletFactory and property-record view (weight, sprite name, sprite, cook flag; the fragments' view names are typedefs of the record structs); the helper template's members defined once at the end of the file and instantiated by the factory's calls (weak, thrown then projectile, as the original; explicit instantiation made them global)." "Merge bulletfactory.cpp: 7 fragments into one unit" "bulletfactory.cpp's whole file in one unit: CBulletFactory's sprites, parameters, queries, creation/destruction, Init/Reset/Shutdown, destructor and constructor, and the weak CBulletFactoryHelper members for thrown and projectile bullets (implicitly instantiated). Complete image identical." ""
sleep 60
one() { # id src tpl origfile idx frags bytes desc [extra]
  sh $T/land3.sh $1 $2 $T/tpl_$3.json $4 $5 "$6" "- 2026-11-13 [loop merge6] merged $4 ($(echo $6 | wc -w) fragments, $7 B): $8" "Merge $4: $(echo $6 | wc -w) fragments into one unit" "$4's whole file in one unit ($6). $8 Complete image identical." "$9"
}
sleep 60
export EDITCMD='sed -i "s/\"unit\": \"weaponfactory_params\"/\"unit\": \"weaponfactory\"/" config/GMFE69/extab_patches.json'
one weaponfactory src/game/weaponfactory.cpp wf_def weaponfactory.cpp 8328 "weaponfactory_params weaponfactory_create weaponfactory_props weaponfactory" 4384 "deferred inlining, written last to first: GetWeaponPropertiesByCRC is defined once and inlined into the earlier CreateWeaponByCRC (no more file duplicate); one CWeaponFactory/CWeapon/CStaticMesh view (the props fragment's, plus the params members); the mesh-loading pass keeps its own record view; EndianSwap reads the record through a byte pointer; the LoadWeaponObjects extab patch moves to the weaponfactory unit."
unset EDITCMD
echo BATCH3 DONE
