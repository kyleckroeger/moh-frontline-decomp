// g_BIFCRCArray (.data 0x80181e88, 0x900 bytes): the script engine's built-in function
// table, 288 entries of {name CRC, handler}. BSInitBIFuncs (research/drafts/
// bsbifunc_setup.cpp) matches each built-in a script file imports by CRC and stores the
// handler. The CRC is the standard CRC-32 (zlib's crc32) of the built-in's name in
// upper case, without the "BIFunc_" prefix: 0x12f32503 = crc32("AIGETSOLDIERMATCHLISTSTRUCT").
// That holds for all 288 entries, so a script's import list names its built-ins.
// Generated from the original's .rela.data; checked by research/tools/check_data.py.
// settings: config/GMFE69/bsbifunc_ai.json

void BIFunc_AEMSEvent(int**, void*);
void BIFunc_AEMSSetMasterVolume(int**, void*);
void BIFunc_AIArrivedAtCoverpoint(int**, void*);
void BIFunc_AIChooseCoverPoint(int**, void*);
void BIFunc_AIChooseMMGPoint(int**, void*);
void BIFunc_AIChooseTarget(int**, void*);
void BIFunc_AIChooseTargetByID(int**, void*);
void BIFunc_AICollisionTest(int**, void*);
void BIFunc_AICommitCoverPointWalkToStored(int**, void*);
void BIFunc_AIContinueAStarPathWalk(int**, void*);
void BIFunc_AIContinueCoverPointWalk(int**, void*);
void BIFunc_AIContinueMovePointWalk(int**, void*);
void BIFunc_AIContinueOrientTowardsCoverPoint(int**, void*);
void BIFunc_AIContinueSplinePathWalk(int**, void*);
void BIFunc_AIEnableTargetAim(int**, void*);
void BIFunc_AIEngageObstacleAvoidance(int**, void*);
void BIFunc_AIFireAtCurrentTarget(int**, void*);
void BIFunc_AIFireMMGAtCurrentTarget(int**, void*);
void BIFunc_AIGetClosestCoverPoint(int**, void*);
void BIFunc_AIGetDistanceToMovePoint(int**, void*);
void BIFunc_AIGetDistanceToTargetXY(int**, void*);
void BIFunc_AIGetDistanceToTargetXYZ(int**, void*);
void BIFunc_AIGetSoldierMatchListStruct(int**, void*);
void BIFunc_AIGetSoldierPointer(int**, void*);
void BIFunc_AIInvalidateCoverPointByID(int**, void*);
void BIFunc_AIIsItAGoodTimeToFire(int**, void*);
void BIFunc_AILeaveCoverPoint(int**, void*);
void BIFunc_AIResetTargetMatchList(int**, void*);
void BIFunc_AISendFlag(int**, void*);
void BIFunc_AISetCoverPoint(int**, void*);
void BIFunc_AISetHearingRadius(int**, void*);
void BIFunc_AISetMovePoint(int**, void*);
void BIFunc_AISetRunAwayMovepoint(int**, void*);
void BIFunc_AISetStatusVar(int**, void*);
void BIFunc_AISetTargetMode(int**, void*);
void BIFunc_AISetupTargetMatchList(int**, void*);
void BIFunc_AISnapToCoverPointPosition(int**, void*);
void BIFunc_AISnapToMovePoint(int**, void*);
void BIFunc_AISnapToMovePointAndAngle(int**, void*);
void BIFunc_AIStartAStarPathWalk(int**, void*);
void BIFunc_AIStartAStarPathWalkToMovePoint(int**, void*);
void BIFunc_AIStartAStarPathWalkWithCurrentTarget(int**, void*);
void BIFunc_AIStartCoverPointWalk(int**, void*);
void BIFunc_AIStartCoverPointWalkToStored(int**, void*);
void BIFunc_AIStartMovePointWalk(int**, void*);
void BIFunc_AIStartOrientTowardsCoverPoint(int**, void*);
void BIFunc_AIStartSplinePathWalk(int**, void*);
void BIFunc_AIStopAStarPathWalk(int**, void*);
void BIFunc_AIStopCoverPointWalk(int**, void*);
void BIFunc_AIStopMovePointWalk(int**, void*);
void BIFunc_AIStopOrientTowardsCoverPoint(int**, void*);
void BIFunc_AIStopSplinePathWalk(int**, void*);
void BIFunc_AIStoreBestCoverPoint(int**, void*);
void BIFunc_AddBuddy(int**, void*);
void BIFunc_AddHint(int**, void*);
void BIFunc_AddObjective(int**, void*);
void BIFunc_AddPlayerAmmo(int**, void*);
void BIFunc_AlarmIsAvailable(int**, void*);
void BIFunc_AmbientTrack_Azimuth(int**, void*);
void BIFunc_AmbientTrack_Select(int**, void*);
void BIFunc_AmbientTrack_Volume(int**, void*);
void BIFunc_ApplyForceToObject(int**, void*);
void BIFunc_AttachObjectToFloatingBone(int**, void*);
void BIFunc_AttachPlayerToMountedMachineGun(int**, void*);
void BIFunc_AttachPlayerToProp(int**, void*);
void BIFunc_AttachToProp(int**, void*);
void BIFunc_AttachToSoldier(int**, void*);
void BIFunc_AttachToSoldierEmptyHand(int**, void*);
void BIFunc_AttachToWaypoint(int**, void*);
void BIFunc_BlockNewVibrations(int**, void*);
void BIFunc_BreakPoint(int**, void*);
void BIFunc_BuddyDied(int**, void*);
void BIFunc_CanICreateChild(int**, void*);
void BIFunc_CatchGrenade(int**, void*);
void BIFunc_ChangePlayersTeam(int**, void*);
void BIFunc_CreateBuddyDataStructures(int**, void*);
void BIFunc_CreateChild(int**, void*);
void BIFunc_CreateChildFallingObject(int**, void*);
void BIFunc_CreateChildRootHierObject(int**, void*);
void BIFunc_CreateChildSubHierObject(int**, void*);
void BIFunc_CreateChildThrownObject(int**, void*);
void BIFunc_CreateDumpObject(int**, void*);
void BIFunc_CreateExplosion(int**, void*);
void BIFunc_CreateImpact(int**, void*);
void BIFunc_CreateLightByType(int**, void*);
void BIFunc_CreateParticleSystemByType(int**, void*);
void BIFunc_DebugInitFromTrigger(int**, void*);
void BIFunc_DebugTTYPrint(int**, void*);
void BIFunc_DestroyLight(int**, void*);
void BIFunc_DestroyObject(int**, void*);
void BIFunc_DestroyParticleSystem(int**, void*);
void BIFunc_DestroySubHierObject(int**, void*);
void BIFunc_DestroyThrownObject(int**, void*);
void BIFunc_DetachFromProp(int**, void*);
void BIFunc_DetachFromSoldier(int**, void*);
void BIFunc_DetachFromWaypoint(int**, void*);
void BIFunc_DetachObjectFromFloatingBone(int**, void*);
void BIFunc_DetachPlayerFromProp(int**, void*);
void BIFunc_DisableCollision(int**, void*);
void BIFunc_DisableFloatingBone(int**, void*);
void BIFunc_DisablePlayerShoot(int**, void*);
void BIFunc_DisplayLetterbox(int**, void*);
void BIFunc_DisplayPrompt(int**, void*);
void BIFunc_DisplayPromptInteger(int**, void*);
void BIFunc_DisplayPromptText(int**, void*);
void BIFunc_DoDamageIfLocationMatches(int**, void*);
void BIFunc_DoSomethingInteresting(int**, void*);
void BIFunc_DropFallingObject(int**, void*);
void BIFunc_EjectHelmet(int**, void*);
void BIFunc_EnableFloatingBone(int**, void*);
void BIFunc_EnableNonVisDestruct(int**, void*);
void BIFunc_EndLevel(int**, void*);
void BIFunc_EndSound(int**, void*);
void BIFunc_FadeOut(int**, void*);
void BIFunc_FireProjectile(int**, void*);
void BIFunc_FireProjectileAtPath(int**, void*);
void BIFunc_FloatToInt(int**, void*);
void BIFunc_GetBSObject(int**, void*);
void BIFunc_GetClosestAlarm(int**, void*);
void BIFunc_GetClosestValidWaypoint(int**, void*);
void BIFunc_GetConstrainedRotateAngle(int**, void*);
void BIFunc_GetEmbeddedHitReactionInfo(int**, void*);
void BIFunc_GetEventSystemMemBlock(int**, void*);
void BIFunc_GetGenericSoundStructureMemory(int**, void*);
void BIFunc_GetGrenadeHoldTime(int**, void*);
void BIFunc_GetGrenadeTimer(int**, void*);
void BIFunc_GetInitialBuddyOrders(int**, void*);
void BIFunc_GetMyInstanceData(int**, void*);
void BIFunc_GetMySoundInfoStruct(int**, void*);
void BIFunc_GetNoiseInfo(int**, void*);
void BIFunc_GetObjInfo(int**, void*);
void BIFunc_GetObjective(int**, void*);
void BIFunc_GetPlayerAmmo(int**, void*);
void BIFunc_GetPropById(int**, void*);
void BIFunc_GetRandomFloat(int**, void*);
void BIFunc_GetRandomInteger(int**, void*);
void BIFunc_GetSoundInfo(int**, void*);
void BIFunc_GetVoiceHandle(int**, void*);
void BIFunc_GetWayPointInfo(int**, void*);
void BIFunc_GetWeaponInfo(int**, void*);
void BIFunc_GetWeaponType(int**, void*);
void BIFunc_GiveEnemyGrenadeWeapon(int**, void*);
void BIFunc_HierObjectFire(int**, void*);
void BIFunc_HierObjectGetAimAngle(int**, void*);
void BIFunc_HierObjectMapWeapon(int**, void*);
void BIFunc_HierObjectPlayAnimation(int**, void*);
void BIFunc_HierObjectRegisterBulletDamage(int**, void*);
void BIFunc_HierObjectSetLookObject(int**, void*);
void BIFunc_InduceGroundCheck(int**, void*);
void BIFunc_InitToInvalid(int**, void*);
void BIFunc_IsAlarmActivated(int**, void*);
void BIFunc_IsPlayerAiming(int**, void*);
void BIFunc_IsPlayerCrouching(int**, void*);
void BIFunc_IsPlayerDisguised(int**, void*);
void BIFunc_IsPlayerInvincible(int**, void*);
void BIFunc_LoadOneShotSoundBank(int**, void*);
void BIFunc_MPAddPlayerAmmo(int**, void*);
void BIFunc_MPAddPlayerHealth(int**, void*);
void BIFunc_MPCreateAmmoByType(int**, void*);
void BIFunc_MPCreateAmmoByWeaponSetID(int**, void*);
void BIFunc_MPCreateWeaponByType(int**, void*);
void BIFunc_MPCreateWeaponByWeaponSetID(int**, void*);
void BIFunc_MPGetAmmoPickupPromptFromWeaponSetID(int**, void*);
void BIFunc_MPGetPlayerBody(int**, void*);
void BIFunc_MPGetPlayerVelocity(int**, void*);
void BIFunc_MPGetWeaponPickupPromptFromWeaponSetID(int**, void*);
void BIFunc_MPGetWeaponTypeFromWeaponSetID(int**, void*);
void BIFunc_MakeNoise(int**, void*);
void BIFunc_MakeNoiseFromInputTrigger(int**, void*);
void BIFunc_MechFollowPath(int**, void*);
void BIFunc_MechSetFlag(int**, void*);
void BIFunc_MechSetPathFlag(int**, void*);
void BIFunc_ModifyTotalKillableEnemyCount(int**, void*);
void BIFunc_MovingMechForceUpdateTrigger(int**, void*);
void BIFunc_PathMechAllowCollisionDamage(int**, void*);
void BIFunc_PathfinderEvent(int**, void*);
void BIFunc_PathfinderFadeVolume(int**, void*);
void BIFunc_PathfinderSetLatency(int**, void*);
void BIFunc_PathfinderSetLevel(int**, void*);
void BIFunc_PlayAnimation(int**, void*);
void BIFunc_PlayAnimationNoTranslation(int**, void*);
void BIFunc_PlayAnimationOnOther(int**, void*);
void BIFunc_PlayFacialAnimation(int**, void*);
void BIFunc_PlayLocomotion(int**, void*);
void BIFunc_PlaySound(int**, void*);
void BIFunc_PlaySoundAtOffset(int**, void*);
void BIFunc_PlaySoundFromInputTrigger(int**, void*);
void BIFunc_PlayVibrationEvent(int**, void*);
void BIFunc_PlayerAddHitDirection(int**, void*);
void BIFunc_PlayerCanCookGrenades(int**, void*);
void BIFunc_PlayerCanCrouch(int**, void*);
void BIFunc_PlayerCanSwitchWeapon(int**, void*);
void BIFunc_PlayerClearReloading(int**, void*);
void BIFunc_PlayerDeath(int**, void*);
void BIFunc_PlayerEjectShell(int**, void*);
void BIFunc_PlayerFinishedReloading(int**, void*);
void BIFunc_PlayerFireMyWeapon(int**, void*);
void BIFunc_PlayerForceToWeapon(int**, void*);
void BIFunc_PlayerGetWeaponRefireRate(int**, void*);
void BIFunc_PlayerHasMeleeWeapon(int**, void*);
void BIFunc_PlayerIsMyWeaponAProjectile(int**, void*);
void BIFunc_PlayerMotionShake(int**, void*);
void BIFunc_PlayerReloadSingle(int**, void*);
void BIFunc_PlayerRespawn(int**, void*);
void BIFunc_PlayerScreenFlash(int**, void*);
void BIFunc_PlayerStartCameraShake(int**, void*);
void BIFunc_PlayerStartGrenadeCook(int**, void*);
void BIFunc_PlayerStartPath(int**, void*);
void BIFunc_PlayerStartReloading(int**, void*);
void BIFunc_PlayerStopCameraShake(int**, void*);
void BIFunc_PlayerStopPath(int**, void*);
void BIFunc_PlayerSuperAimMode(int**, void*);
void BIFunc_PlayerSwitchWeapons(int**, void*);
void BIFunc_RegisterAlarm(int**, void*);
void BIFunc_RegisterBuddyPlayerCheck(int**, void*);
void BIFunc_RegisterCorpseSearch(int**, void*);
void BIFunc_RegisterDistanceToTarget(int**, void*);
void BIFunc_RegisterMMGCheck(int**, void*);
void BIFunc_RegisterTargetCheck(int**, void*);
void BIFunc_RegisterTargetSearch(int**, void*);
void BIFunc_RegisterTimerEvent(int**, void*);
void BIFunc_ResetMMG(int**, void*);
void BIFunc_ResetParticleSystemRotation(int**, void*);
void BIFunc_RotateRel(int**, void*);
void BIFunc_SEDEvent(int**, void*);
void BIFunc_ScaleRel(int**, void*);
void BIFunc_ScriptError(int**, void*);
void BIFunc_SendEvent(int**, void*);
void BIFunc_SendEventDelayed(int**, void*);
void BIFunc_SendEventImmediate(int**, void*);
void BIFunc_SendEventParent(int**, void*);
void BIFunc_SendEventSelf(int**, void*);
void BIFunc_SendMessage(int**, void*);
void BIFunc_SetAIObjectSharedDataPropVars(int**, void*);
void BIFunc_SetAlarmActivated(int**, void*);
void BIFunc_SetAlarmOccupy(int**, void*);
void BIFunc_SetBlinkState(int**, void*);
void BIFunc_SetBuddyPathGroup(int**, void*);
void BIFunc_SetCanBePushed(int**, void*);
void BIFunc_SetCollisionToPlayer(int**, void*);
void BIFunc_SetDefaultFogParams(int**, void*);
void BIFunc_SetDetonationTime(int**, void*);
void BIFunc_SetExpression(int**, void*);
void BIFunc_SetFogParams(int**, void*);
void BIFunc_SetGravity(int**, void*);
void BIFunc_SetGrenadeSlot(int**, void*);
void BIFunc_SetGrenadeTimer(int**, void*);
void BIFunc_SetGroundCheck(int**, void*);
void BIFunc_SetHDCollision(int**, void*);
void BIFunc_SetHeadAsRoot(int**, void*);
void BIFunc_SetHeadTrackPlayer(int**, void*);
void BIFunc_SetInShadow(int**, void*);
void BIFunc_SetJawMotion(int**, void*);
void BIFunc_SetMGWaypointFlag(int**, void*);
void BIFunc_SetMechanicCollision(int**, void*);
void BIFunc_SetMood(int**, void*);
void BIFunc_SetMuzzleFireForPlayer(int**, void*);
void BIFunc_SetMySoundInfoStruct(int**, void*);
void BIFunc_SetObjInfo(int**, void*);
void BIFunc_SetObjective(int**, void*);
void BIFunc_SetParticleSystemVelocity(int**, void*);
void BIFunc_SetPlayerCollision(int**, void*);
void BIFunc_SetPlayerFallingDamage(int**, void*);
void BIFunc_SetPlayerWielding(int**, void*);
void BIFunc_SetSingleAnimEndsEventDelivery(int**, void*);
void BIFunc_SetWeaponDraw(int**, void*);
void BIFunc_SetWeaponEnable(int**, void*);
void BIFunc_SetupGrenadeShot(int**, void*);
void BIFunc_StartGrenadeHold(int**, void*);
void BIFunc_StartMotionPlayback(int**, void*);
void BIFunc_StaticObjectPlayAnimation(int**, void*);
void BIFunc_StopBuddyPlayerCheck(int**, void*);
void BIFunc_StopCorpseSearch(int**, void*);
void BIFunc_StopCurrentVoice(int**, void*);
void BIFunc_StopDistanceToTarget(int**, void*);
void BIFunc_StopGrenadeHold(int**, void*);
void BIFunc_StopMMGCheck(int**, void*);
void BIFunc_StopMotionPlayback(int**, void*);
void BIFunc_StopTargetCheck(int**, void*);
void BIFunc_StopTargetSearch(int**, void*);
void BIFunc_SwapHierObject(int**, void*);
void BIFunc_TogglePlayerVsTriggerChecks(int**, void*);
void BIFunc_TranslateRel(int**, void*);
void BIFunc_UnregisterAlarm(int**, void*);
void BIFunc_UpdateBuddyCheckpoints(int**, void*);
void BIFunc_UpdateBuddyProgress(int**, void*);
void BIFunc_UpdateFragCount(int**, void*);
void BIFunc_UpdatePlayerStats(int**, void*);

struct BIFCRCEntry {
    unsigned int m_crc;
    void (*m_function)(int**, void*);
};

BIFCRCEntry g_BIFCRCArray[288] = {
    {0x12f32503, BIFunc_AIGetSoldierMatchListStruct},
    {0xcc919301, BIFunc_AIGetSoldierPointer},
    {0x964da7fb, BIFunc_AISetupTargetMatchList},
    {0xd1644323, BIFunc_AIResetTargetMatchList},
    {0x627f97d7, BIFunc_AISetTargetMode},
    {0xd8716f9e, BIFunc_SetAIObjectSharedDataPropVars},
    {0x2f8a0d98, BIFunc_RegisterDistanceToTarget},
    {0x21c8d9a4, BIFunc_RegisterTargetSearch},
    {0xbbc62dfb, BIFunc_RegisterTargetCheck},
    {0xb8e276bf, BIFunc_RegisterCorpseSearch},
    {0xe4192feb, BIFunc_RegisterBuddyPlayerCheck},
    {0x6b064677, BIFunc_RegisterMMGCheck},
    {0xba10c813, BIFunc_StopDistanceToTarget},
    {0x7bece1ac, BIFunc_StopTargetSearch},
    {0x9a213e05, BIFunc_StopTargetCheck},
    {0xe2c64eb7, BIFunc_StopCorpseSearch},
    {0x7183ea60, BIFunc_StopBuddyPlayerCheck},
    {0x6d559d81, BIFunc_StopMMGCheck},
    {0xb8361e70, BIFunc_AIChooseTarget},
    {0x121faf7d, BIFunc_AIChooseTargetByID},
    {0x78db2174, BIFunc_AISetHearingRadius},
    {0x210f63a1, BIFunc_AIGetDistanceToTargetXY},
    {0xf84ccb7a, BIFunc_AIGetDistanceToTargetXYZ},
    {0x6bba973d, BIFunc_AIEnableTargetAim},
    {0xa739f867, BIFunc_AIIsItAGoodTimeToFire},
    {0xb3fb9a02, BIFunc_AIFireAtCurrentTarget},
    {0xd9b324f1, BIFunc_AIFireMMGAtCurrentTarget},
    {0xf16918d9, BIFunc_FireProjectile},
    {0x3359980c, BIFunc_FireProjectileAtPath},
    {0x140a2ff2, BIFunc_HierObjectSetLookObject},
    {0x15ebdf6a, BIFunc_HierObjectMapWeapon},
    {0xc42e27e3, BIFunc_HierObjectGetAimAngle},
    {0x2bba76bf, BIFunc_HierObjectFire},
    {0x92a89039, BIFunc_HierObjectRegisterBulletDamage},
    {0xd4b2fc4f, BIFunc_GetConstrainedRotateAngle},
    {0xa0cebc0b, BIFunc_DoDamageIfLocationMatches},
    {0x1861c7fa, BIFunc_AIStartSplinePathWalk},
    {0xa9c7c76e, BIFunc_AIStopSplinePathWalk},
    {0x5334541a, BIFunc_AIContinueSplinePathWalk},
    {0x0fdbdabf, BIFunc_AIStartAStarPathWalk},
    {0x0a8edb49, BIFunc_AIStartAStarPathWalkWithCurrentTarget},
    {0xa2326a84, BIFunc_AIStopAStarPathWalk},
    {0x5e9c7d9a, BIFunc_AIContinueAStarPathWalk},
    {0x70abd31d, BIFunc_AIEngageObstacleAvoidance},
    {0xe7c1cba0, BIFunc_GetWayPointInfo},
    {0x1d1e1a02, BIFunc_AIStoreBestCoverPoint},
    {0x54ce5b0e, BIFunc_AIGetClosestCoverPoint},
    {0x4b7d4fe5, BIFunc_AIStartCoverPointWalk},
    {0xf098e139, BIFunc_AIStartCoverPointWalkToStored},
    {0xb86e2db5, BIFunc_AICommitCoverPointWalkToStored},
    {0xfadb4f71, BIFunc_AIStopCoverPointWalk},
    {0x0028dc05, BIFunc_AIContinueCoverPointWalk},
    {0xc0e04120, BIFunc_AIArrivedAtCoverpoint},
    {0x881551db, BIFunc_AILeaveCoverPoint},
    {0xe3e5daf6, BIFunc_AIStartOrientTowardsCoverPoint},
    {0x8171ef40, BIFunc_AIStopOrientTowardsCoverPoint},
    {0x6caed4b0, BIFunc_AIContinueOrientTowardsCoverPoint},
    {0x632749da, BIFunc_AISnapToCoverPointPosition},
    {0x65b6e865, BIFunc_AIInvalidateCoverPointByID},
    {0x95845412, BIFunc_GetClosestValidWaypoint},
    {0xc8d951b3, BIFunc_AIChooseMMGPoint},
    {0x2d335aaa, BIFunc_AISetMovePoint},
    {0x722bc49a, BIFunc_AIStartMovePointWalk},
    {0xdfc274a1, BIFunc_AIStopMovePointWalk},
    {0x236c63bf, BIFunc_AIContinueMovePointWalk},
    {0x4859de77, BIFunc_AISnapToMovePoint},
    {0x2f7481c6, BIFunc_AISnapToMovePointAndAngle},
    {0x84e500c0, BIFunc_AIGetDistanceToMovePoint},
    {0x4140ae20, BIFunc_AIStartAStarPathWalkToMovePoint},
    {0x5b939840, BIFunc_AIChooseCoverPoint},
    {0xa99221a5, BIFunc_AISetCoverPoint},
    {0x39d6ed49, BIFunc_AISetRunAwayMovepoint},
    {0x4ec96eb2, BIFunc_PlayAnimation},
    {0x5d8aae9f, BIFunc_PlayLocomotion},
    {0xe8892478, BIFunc_PlayAnimationOnOther},
    {0xc81719ba, BIFunc_PlayFacialAnimation},
    {0x04be72f0, BIFunc_PlayAnimationNoTranslation},
    {0xff3071c7, BIFunc_AICollisionTest},
    {0x0cf4ea8a, BIFunc_SetGravity},
    {0xc67460c7, BIFunc_SetGroundCheck},
    {0x63312223, BIFunc_InduceGroundCheck},
    {0x17d2a2c5, BIFunc_SetHDCollision},
    {0x10937c0e, BIFunc_DisableCollision},
    {0xb338cca7, BIFunc_SetMechanicCollision},
    {0xe7f48a50, BIFunc_SetCanBePushed},
    {0x5ba09dd4, BIFunc_PathMechAllowCollisionDamage},
    {0x2d1a465f, BIFunc_SetHeadAsRoot},
    {0xcc5908ec, BIFunc_SetCollisionToPlayer},
    {0x468edcc0, BIFunc_SendEvent},
    {0x85bfa7e5, BIFunc_SendEventSelf},
    {0xe90a1b5f, BIFunc_SendEventParent},
    {0x56e2c2eb, BIFunc_SendEventImmediate},
    {0x4a8cfee0, BIFunc_SendEventDelayed},
    {0x5d5bc3b4, BIFunc_SendMessage},
    {0x0b379420, BIFunc_GetEventSystemMemBlock},
    {0x07b7fc8d, BIFunc_RegisterTimerEvent},
    {0x54ed2131, BIFunc_EnableNonVisDestruct},
    {0x9a32c852, BIFunc_GetMyInstanceData},
    {0x8f774186, BIFunc_GetEmbeddedHitReactionInfo},
    {0xe44d223b, BIFunc_GetObjInfo},
    {0xca0803a9, BIFunc_SetObjInfo},
    {0x24c0749a, BIFunc_GetWeaponType},
    {0xfedb1263, BIFunc_SetWeaponEnable},
    {0x620db9b9, BIFunc_GetRandomInteger},
    {0x796b0cde, BIFunc_GetRandomFloat},
    {0x9cfecb1b, BIFunc_GetMySoundInfoStruct},
    {0x32d58b67, BIFunc_SetMySoundInfoStruct},
    {0xd9e19d6e, BIFunc_GetNoiseInfo},
    {0x6351c832, BIFunc_TranslateRel},
    {0xa58b3453, BIFunc_RotateRel},
    {0xa3f9cc03, BIFunc_ScaleRel},
    {0x777723a8, BIFunc_InitToInvalid},
    {0xcfcb99aa, BIFunc_DisplayPrompt},
    {0x1635adbc, BIFunc_DisplayLetterbox},
    {0x3a8e0b07, BIFunc_AddObjective},
    {0x3c8d264c, BIFunc_SetObjective},
    {0x3393f0cf, BIFunc_GetObjective},
    {0x65bd26bb, BIFunc_AddHint},
    {0xbe056ba6, BIFunc_FloatToInt},
    {0x73342bb3, BIFunc_CreateChild},
    {0x78e03854, BIFunc_DestroyObject},
    {0x22f18e41, BIFunc_CanICreateChild},
    {0x3852a2f2, BIFunc_CreateChildThrownObject},
    {0xd7ab0bb9, BIFunc_DestroyThrownObject},
    {0x31a4a1d5, BIFunc_CreateChildFallingObject},
    {0xd94717ae, BIFunc_DropFallingObject},
    {0xb4d3fcca, BIFunc_CreateChildRootHierObject},
    {0xa48346dc, BIFunc_CreateChildSubHierObject},
    {0xeeac8bc5, BIFunc_SwapHierObject},
    {0x4562276d, BIFunc_DestroySubHierObject},
    {0x654bc7e2, BIFunc_CreateParticleSystemByType},
    {0x39dcb6b7, BIFunc_ResetParticleSystemRotation},
    {0x7551c57b, BIFunc_SetParticleSystemVelocity},
    {0x27515812, BIFunc_DestroyParticleSystem},
    {0x0983d290, BIFunc_CreateLightByType},
    {0x469f1332, BIFunc_DestroyLight},
    {0x273f0933, BIFunc_CreateDumpObject},
    {0x98fdec8d, BIFunc_GetPlayerAmmo},
    {0x0d4bbbf4, BIFunc_AddPlayerAmmo},
    {0xb51df512, BIFunc_PlayerEjectShell},
    {0x3e80717f, BIFunc_DisablePlayerShoot},
    {0x6bd753b6, BIFunc_SetPlayerCollision},
    {0xac5252f0, BIFunc_SetWeaponDraw},
    {0x5281731a, BIFunc_PlayerIsMyWeaponAProjectile},
    {0x9839fedc, BIFunc_PlayerFireMyWeapon},
    {0x1a4e1a2b, BIFunc_PlayerStartReloading},
    {0xc346e9e3, BIFunc_PlayerReloadSingle},
    {0xabf4ba02, BIFunc_PlayerClearReloading},
    {0x46945630, BIFunc_PlayerFinishedReloading},
    {0xdef42097, BIFunc_PlayerSwitchWeapons},
    {0x957b67ee, BIFunc_PlayerDeath},
    {0x13e17ef7, BIFunc_PlayerGetWeaponRefireRate},
    {0xe92c0317, BIFunc_PlayerAddHitDirection},
    {0xe7cce2f1, BIFunc_PlayerCanSwitchWeapon},
    {0x7d6a0760, BIFunc_PlayerCanCookGrenades},
    {0x20dcbaf5, BIFunc_PlayerHasMeleeWeapon},
    {0x111337af, BIFunc_IsPlayerAiming},
    {0x1b0a9e98, BIFunc_IsPlayerDisguised},
    {0x8c8a2096, BIFunc_IsPlayerCrouching},
    {0x72a819b5, BIFunc_IsPlayerInvincible},
    {0xad078995, BIFunc_SetPlayerWielding},
    {0x691ee6c9, BIFunc_SetPlayerFallingDamage},
    {0xa8f1cbd7, BIFunc_PlayerRespawn},
    {0x355829ba, BIFunc_GetGenericSoundStructureMemory},
    {0xa638f607, BIFunc_GetSoundInfo},
    {0xea6c53a1, BIFunc_PlaySound},
    {0x4d5aa397, BIFunc_EndSound},
    {0x2637782c, BIFunc_PlaySoundAtOffset},
    {0xd36cd3fe, BIFunc_PlaySoundFromInputTrigger},
    {0x36a906e2, BIFunc_SEDEvent},
    {0xeb839e89, BIFunc_AEMSEvent},
    {0xd941efac, BIFunc_PathfinderSetLevel},
    {0xc0d84ad7, BIFunc_PathfinderEvent},
    {0xc3ba7e0e, BIFunc_PathfinderFadeVolume},
    {0x58e8f73c, BIFunc_PathfinderSetLatency},
    {0xf9a52b55, BIFunc_MakeNoise},
    {0x2842faf8, BIFunc_MakeNoiseFromInputTrigger},
    {0x86004007, BIFunc_GetVoiceHandle},
    {0x8ce20cdd, BIFunc_StopCurrentVoice},
    {0xd112dfab, BIFunc_CreateBuddyDataStructures},
    {0x8b49b6fe, BIFunc_AddBuddy},
    {0x8b2bd60e, BIFunc_GetInitialBuddyOrders},
    {0x8da731cb, BIFunc_BuddyDied},
    {0x96690017, BIFunc_UpdateBuddyProgress},
    {0xa9e23d9b, BIFunc_UpdateBuddyCheckpoints},
    {0xdb65b79b, BIFunc_SetBuddyPathGroup},
    {0xb8bddedf, BIFunc_DebugTTYPrint},
    {0x4d412cd6, BIFunc_ScriptError},
    {0xad26b30a, BIFunc_DoSomethingInteresting},
    {0x452c5879, BIFunc_BreakPoint},
    {0x32952724, BIFunc_PlayerStartCameraShake},
    {0x4a90f4bf, BIFunc_PlayerStopCameraShake},
    {0x7d1760b3, BIFunc_PlayerSuperAimMode},
    {0x3e612cca, BIFunc_PlayerCanCrouch},
    {0x297bf47f, BIFunc_PlayerMotionShake},
    {0x94caa3f6, BIFunc_PlayerScreenFlash},
    {0x01aecd70, BIFunc_PlayerStartPath},
    {0x8b2ed074, BIFunc_PlayerStopPath},
    {0x1f77e7c9, BIFunc_PlayerStartGrenadeCook},
    {0xfedfe1c9, BIFunc_DisplayPromptText},
    {0x0c13b920, BIFunc_DisplayPromptInteger},
    {0x735d53e8, BIFunc_AISendFlag},
    {0xa9bda7aa, BIFunc_AISetStatusVar},
    {0x3464b177, BIFunc_LoadOneShotSoundBank},
    {0x639712e4, BIFunc_GetWeaponInfo},
    {0x44972e0b, BIFunc_AttachToProp},
    {0xac1a7982, BIFunc_DetachFromProp},
    {0xd1716aab, BIFunc_AttachPlayerToProp},
    {0x71b4af7b, BIFunc_DetachPlayerFromProp},
    {0xf723b56b, BIFunc_AttachToSoldier},
    {0x7f34a182, BIFunc_DetachFromSoldier},
    {0xbc9bcd2d, BIFunc_AttachToWaypoint},
    {0x65c580e5, BIFunc_DetachFromWaypoint},
    {0xc70c360b, BIFunc_AttachToSoldierEmptyHand},
    {0xe6d2e978, BIFunc_GetPropById},
    {0xebbcb029, BIFunc_AmbientTrack_Select},
    {0x19d49737, BIFunc_AmbientTrack_Volume},
    {0xff624707, BIFunc_AmbientTrack_Azimuth},
    {0xca318634, BIFunc_AEMSSetMasterVolume},
    {0x52dfffce, BIFunc_SetupGrenadeShot},
    {0x4ebf673d, BIFunc_CatchGrenade},
    {0xd1aa7e61, BIFunc_GetGrenadeTimer},
    {0x1fb4844d, BIFunc_SetGrenadeTimer},
    {0xafa4973f, BIFunc_GiveEnemyGrenadeWeapon},
    {0x09bf42de, BIFunc_SetMood},
    {0xebb65033, BIFunc_SetExpression},
    {0xace69eee, BIFunc_SetBlinkState},
    {0xdbed8612, BIFunc_SetJawMotion},
    {0xcc745189, BIFunc_SetMuzzleFireForPlayer},
    {0xd5c54bc3, BIFunc_UpdateFragCount},
    {0x0c31d8c4, BIFunc_UpdatePlayerStats},
    {0x5bdb25c8, BIFunc_HierObjectPlayAnimation},
    {0x548fb1cd, BIFunc_StaticObjectPlayAnimation},
    {0xf3dccbc9, BIFunc_StartMotionPlayback},
    {0xd413fa51, BIFunc_StopMotionPlayback},
    {0x2f3eac00, BIFunc_EndLevel},
    {0xe76f6e9d, BIFunc_AttachPlayerToMountedMachineGun},
    {0xa2f7eb85, BIFunc_SetGrenadeSlot},
    {0x9652e0d5, BIFunc_GetGrenadeHoldTime},
    {0x87ef51f3, BIFunc_StartGrenadeHold},
    {0xd42bc517, BIFunc_StopGrenadeHold},
    {0xa2fccdeb, BIFunc_CreateExplosion},
    {0xefec1dea, BIFunc_CreateImpact},
    {0x53cb9a6d, BIFunc_GetBSObject},
    {0xa2a416f9, BIFunc_ApplyForceToObject},
    {0xab822024, BIFunc_MechFollowPath},
    {0x2a86c58a, BIFunc_MechSetPathFlag},
    {0x290a77f0, BIFunc_MovingMechForceUpdateTrigger},
    {0x55b117b6, BIFunc_MechSetFlag},
    {0x6e063060, BIFunc_DebugInitFromTrigger},
    {0xbf10d8e0, BIFunc_PlayVibrationEvent},
    {0xcabd719f, BIFunc_BlockNewVibrations},
    {0xaf06e673, BIFunc_FadeOut},
    {0x960e13bc, BIFunc_AttachObjectToFloatingBone},
    {0xf7f6d2f2, BIFunc_DetachObjectFromFloatingBone},
    {0x5436d716, BIFunc_EnableFloatingBone},
    {0x5453739b, BIFunc_DisableFloatingBone},
    {0x2a3733b8, BIFunc_SetDetonationTime},
    {0x86d203b7, BIFunc_SetSingleAnimEndsEventDelivery},
    {0x35bd54e1, BIFunc_SetFogParams},
    {0x4037d02c, BIFunc_SetDefaultFogParams},
    {0xbb77ce51, BIFunc_SetHeadTrackPlayer},
    {0xc1a14ec2, BIFunc_SetMGWaypointFlag},
    {0x070e747a, BIFunc_PlayerForceToWeapon},
    {0xdb912483, BIFunc_RegisterAlarm},
    {0x340a992e, BIFunc_UnregisterAlarm},
    {0x94777a7f, BIFunc_GetClosestAlarm},
    {0x006d68e1, BIFunc_AlarmIsAvailable},
    {0xd7ff37a1, BIFunc_SetAlarmOccupy},
    {0x094ce95b, BIFunc_IsAlarmActivated},
    {0x09b6286c, BIFunc_SetAlarmActivated},
    {0x9a1d5549, BIFunc_EjectHelmet},
    {0xcb5ce5bd, BIFunc_SetInShadow},
    {0xef59bf71, BIFunc_ResetMMG},
    {0xf5db6b1c, BIFunc_ModifyTotalKillableEnemyCount},
    {0x8f0de4a2, BIFunc_ChangePlayersTeam},
    {0xeffb33a0, BIFunc_TogglePlayerVsTriggerChecks},
    {0x0fb928a0, BIFunc_MPCreateWeaponByWeaponSetID},
    {0xfda7339b, BIFunc_MPCreateAmmoByWeaponSetID},
    {0x41f750e0, BIFunc_MPCreateWeaponByType},
    {0xca02a7b6, BIFunc_MPCreateAmmoByType},
    {0xe0c8e4fd, BIFunc_MPGetWeaponPickupPromptFromWeaponSetID},
    {0x22aa672f, BIFunc_MPGetAmmoPickupPromptFromWeaponSetID},
    {0x12283241, BIFunc_MPGetWeaponTypeFromWeaponSetID},
    {0xc4123c7d, BIFunc_MPAddPlayerAmmo},
    {0x0d18e7cb, BIFunc_MPAddPlayerHealth},
    {0x65831e9c, BIFunc_MPGetPlayerBody},
    {0x88e4e169, BIFunc_MPGetPlayerVelocity},
};
