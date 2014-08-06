#include "stdafx.h"
#include "GameServer.h"

RwUInt32		MobActivity::AcquireMOBSerialId(void)
{
	if(m_uiSerialId++)
	{
		if(m_uiSerialId == 0xffffffff)
			m_uiSerialId = 0;
	}

	return m_uiSerialId;
}
/// CREATE MONSTER LIST ///
bool		MobActivity::Create()
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();

	CSpawnTable* pMOBSpawnTbl = app->g_pTableContainer->GetMobSpawnTable( 1 );

	for ( CTable::TABLEIT itMOBSpawn = pMOBSpawnTbl->Begin(); itMOBSpawn != pMOBSpawnTbl->End(); ++itMOBSpawn )
	{
		sSPAWN_TBLDAT* pMOBSpwnTblData = (sSPAWN_TBLDAT*) itMOBSpawn->second;
		sMOB_TBLDAT* pMOBTblData = (sMOB_TBLDAT*)app->g_pTableContainer->GetMobTable()->FindData( pMOBSpwnTblData->mob_Tblidx );

		if ( pMOBTblData )
		{
			CreatureData * cr = new CreatureData;

			cr->Level = pMOBTblData->byLevel;
			cr->CurEP = pMOBTblData->wBasic_EP;
			cr->CurLP = pMOBTblData->wBasic_LP;
			cr->FightMode = false;
			cr->IsDead = false;
			cr->MonsterID = pMOBTblData->tblidx;
			cr->MonsterSpawnID = pMOBSpwnTblData->tblidx;
			cr->Spawn_Loc = pMOBSpwnTblData->vSpawn_Loc;
			cr->Spawn_Dir = pMOBSpwnTblData->vSpawn_Dir;
			cr->MaxEP = pMOBTblData->wBasic_EP;
			cr->MaxLP = pMOBTblData->wBasic_LP;
			cr->Spawn_Quantity = pMOBSpwnTblData->bySpawn_Quantity;
			cr->Move_DelayTime = pMOBSpwnTblData->byMove_DelayTime;
			cr->Run_Speed = pMOBTblData->fRun_Speed;
			cr->Run_Speed_origin = pMOBTblData->fRun_Speed_Origin;
			cr->Walk_Speed = pMOBTblData->fWalk_Speed;
			cr->Walk_Speed_origin = pMOBTblData->fWalk_Speed_Origin;
			cr->Spawn_Cool_Time = pMOBSpwnTblData->wSpawn_Cool_Time * 1000;
			cr->target = 0;
			cr->curPos = cr->Spawn_Loc;
			cr->isAggro = false;
			m_monsterList.push_back(cr);
		}
	}
	return true;
}
void		MobActivity::CreatureData::Attack(PlayerInfos *plr, CGameServer *app)
{
	printf("ATTACK FUNC STARTED\n");
	this->FightMode = true;
	CNtlPacket packet(sizeof(sGU_CHAR_ACTION_ATTACK));
	sGU_CHAR_ACTION_ATTACK * res = (sGU_CHAR_ACTION_ATTACK *)packet.GetPacketData();

	res->wOpCode = GU_CHAR_ACTION_ATTACK;
	res->hSubject = this->MonsterSpawnID;
	res->hTarget = plr->GetAvatarandle();
	res->dwLpEpEventId = 255;
	res->bChainAttack = false;
	res->byBlockedAction = 255;
	res->wAttackResultValue = 100;
	res->fReflectedDamage = 0;
	res->vShift = plr->GetPosition();
	res->byAttackSequence = 1;
	res->bChainAttack = false;
	res->byAttackResult = BATTLE_ATTACK_RESULT_HIT;
	packet.SetPacketLen( sizeof(sGU_CHAR_ACTION_ATTACK) );
	app->UserBroadcast(&packet);
	plr->TakeDamage(res->wAttackResultValue);
}
void		MobActivity::CreatureData::isAggroByPlayer(PlayerInfos *plr, CGameServer *app)
{

		sVECTOR3 myPos;
		myPos.x = this->curPos.x;
		myPos.y = this->curPos.y;
		myPos.z = this->curPos.z;
		float dist = app->mob->Distance(myPos, plr->GetPosition());
		if (dist <= 20 && dist > 1)
		{
			CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_STATE));
			sGU_UPDATE_CHAR_STATE * res = (sGU_UPDATE_CHAR_STATE *)packet.GetPacketData();

			res->wOpCode = GU_UPDATE_CHAR_STATE;
			res->handle = this->MonsterSpawnID;
			res->sCharState.sCharStateBase.bFightMode = true;
			res->sCharState.sCharStateBase.byStateID = CHARSTATE_DESTMOVE;
			res->sCharState.sCharStateDetail.sCharStateDestMove.byDestLocCount = 1;
			res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].x = plr->GetPosition().x - 1;
			res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].y = plr->GetPosition().y - 1;
			res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].z = plr->GetPosition().z - 1;
			res->sCharState.sCharStateDetail.sCharStateDestMove.bHaveSecondDestLoc = false;
			/*
				THIS PART IS NOT GOOD
			*/
			this->curPos = plr->GetPosition();
			sVECTOR3 myPos;
			myPos.x = this->curPos.x;
			myPos.y = this->curPos.y;
			myPos.z = this->curPos.z;
			packet.SetPacketLen( sizeof(sGU_UPDATE_CHAR_STATE) );
			app->UserBroadcast(&packet);
			float dist = app->mob->Distance(myPos, plr->GetPosition());
			/*
				END.
				WE NEED TO CHECK OR MAKE A THREAD FOR CHECK THE CURRENT POSITION BASED ON MOVEMENT AND ALSO SEND THE ATTACK
			*/
			if(dist <= 2)
			{
				printf("SEND ATTACK FUNC()\n");
				this->Attack(plr, app);
			}
		}
		else if (dist > 20)
		{
			this->target = 0;
			this->isAggro = false;
			CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_STATE));
			sGU_UPDATE_CHAR_STATE * res = (sGU_UPDATE_CHAR_STATE *)packet.GetPacketData();

			res->wOpCode = GU_UPDATE_CHAR_STATE;
			res->handle = this->MonsterSpawnID;
			res->sCharState.sCharStateBase.bFightMode = false;
			res->sCharState.sCharStateBase.byStateID = CHARSTATE_DESTMOVE;
			res->sCharState.sCharStateDetail.sCharStateDestMove.byDestLocCount = 1;
			res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].x = this->Spawn_Loc.x;
			res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].y = this->Spawn_Loc.y;
			res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].z = this->Spawn_Loc.z;
			res->sCharState.sCharStateDetail.sCharStateDestMove.bHaveSecondDestLoc = false;
			packet.SetPacketLen( sizeof(sGU_UPDATE_CHAR_STATE) );
			app->UserBroadcast(&packet);
		}
}
/// CREATE MONSTER LIST END ///
float		MobActivity::Distance(const sVECTOR3 mycurPos, const CNtlVector othercurPos)
{
	float first = (mycurPos.x - othercurPos.x) * (mycurPos.x - othercurPos.x);
	float second = (mycurPos.y - othercurPos.y) * (mycurPos.y - othercurPos.y);
	float third = (mycurPos.z - othercurPos.z) * (mycurPos.z - othercurPos.z);
    float distance = sqrt(first + second + third);
    return distance;
}
bool		MobActivity::CreatureRangeCheck(sVECTOR3 mycurPos, CNtlVector othercurPos)
{
	float distance = Distance(mycurPos, othercurPos);
	if (distance < DISTANCE_TO_SPAWN)
	{
		return true;
	}
	return false;
}
MobActivity::CreatureData*	MobActivity::GetMobByHandle(RwUInt32 Target)
{
	CreatureData * creaturelist;

	for (MONSTERLISTIT it = m_monsterList.begin(); it != m_monsterList.end(); ++it )
	{
		creaturelist = (*it);
		if (creaturelist)
		{
			if (creaturelist->MonsterSpawnID == Target)
			{
				return creaturelist;
				break;
			}
		}
	}
	return NULL;
}
bool		MobActivity::RunSpawnCheck(CNtlPacket * pPacket, sVECTOR3 curPos, CClientSession * pSession)
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();

#pragma region monstercheck

	CreatureData * creaturelist;

	for (MONSTERLISTIT it = m_monsterList.begin(); it != m_monsterList.end(); ++it )
	{
		creaturelist = (*it);
		if(app->mob->CreatureRangeCheck(curPos, creaturelist->Spawn_Loc) == true)
		{
			if(pSession->IsMonsterInsideList(creaturelist->MonsterSpawnID) == false)
			{
				if ( creaturelist->IsDead == false )
				{
					CNtlPacket packet(sizeof(sGU_OBJECT_CREATE));
					sGU_OBJECT_CREATE * res = (sGU_OBJECT_CREATE *)packet.GetPacketData();
					creaturelist->isSpawned = true;
					res->wOpCode = GU_OBJECT_CREATE;
					res->sObjectInfo.objType = OBJTYPE_MOB;
					res->handle = creaturelist->MonsterSpawnID;
					res->sObjectInfo.mobState.sCharStateBase.vCurLoc.x = creaturelist->Spawn_Loc.x;
					res->sObjectInfo.mobState.sCharStateBase.vCurLoc.y = creaturelist->Spawn_Loc.y;
					res->sObjectInfo.mobState.sCharStateBase.vCurLoc.z = creaturelist->Spawn_Loc.z;
					res->sObjectInfo.mobState.sCharStateBase.vCurDir.x = creaturelist->Spawn_Dir.x;
					res->sObjectInfo.mobState.sCharStateBase.vCurDir.y = creaturelist->Spawn_Dir.y;
					res->sObjectInfo.mobState.sCharStateBase.vCurDir.z = creaturelist->Spawn_Dir.z;
					res->sObjectInfo.mobState.sCharStateBase.byStateID = CHARSTATE_SPAWNING;
					res->sObjectInfo.mobState.sCharStateBase.bFightMode = creaturelist->FightMode;
					res->sObjectInfo.mobBrief.tblidx = creaturelist->MonsterID;
					res->sObjectInfo.mobBrief.wCurEP = creaturelist->CurEP;
					res->sObjectInfo.mobBrief.wMaxEP = creaturelist->MaxEP;
					res->sObjectInfo.mobBrief.wCurLP = creaturelist->CurLP;
					res->sObjectInfo.mobBrief.wMaxLP = creaturelist->MaxLP;
					res->sObjectInfo.mobBrief.fLastRunningSpeed =  creaturelist->Run_Speed;
					res->sObjectInfo.mobBrief.fLastWalkingSpeed = creaturelist->Walk_Speed;

					pSession->InsertIntoMyMonsterList(creaturelist->MonsterSpawnID, creaturelist->Spawn_Loc, creaturelist->MonsterID);

					packet.SetPacketLen( sizeof(sGU_OBJECT_CREATE) );
					g_pApp->Send( pSession->GetHandle(), &packet );
				}
			}
		}else if(pSession->IsMonsterInsideList(creaturelist->MonsterSpawnID) == true){
			//	printf("remove monster \n");
				CNtlPacket packet(sizeof(sGU_OBJECT_DESTROY));
				sGU_OBJECT_DESTROY * res = (sGU_OBJECT_DESTROY *)packet.GetPacketData();
				res->wOpCode = GU_OBJECT_DESTROY;
				res->handle = creaturelist->MonsterSpawnID;
				creaturelist->isSpawned = false;
				pSession->RemoveFromMyMonsterList(creaturelist->MonsterSpawnID);
	
				packet.SetPacketLen( sizeof(sGU_OBJECT_DESTROY) );
				g_pApp->Send( pSession->GetHandle(), &packet );
			}
	}


#pragma endregion Monstercheckend
#pragma region npccheck

	CSpawnTable* pNPCSpawnTbl = app->g_pTableContainer->GetNpcSpawnTable( pSession->plr->GetWorldID() );
	for ( CTable::TABLEIT itNPCSpawn = pNPCSpawnTbl->Begin(); itNPCSpawn != pNPCSpawnTbl->End(); ++itNPCSpawn )
	{
		sSPAWN_TBLDAT* pNPCSpwnTblData = (sSPAWN_TBLDAT*) itNPCSpawn->second;
		sNPC_TBLDAT* pNPCTblData = (sNPC_TBLDAT*)app->g_pTableContainer->GetNpcTable()->FindData( pNPCSpwnTblData->mob_Tblidx );

		if(pSession->IsNpcInsideList(pNPCSpwnTblData->mob_Tblidx) == false)
		{

			if(app->mob->CreatureRangeCheck(curPos, pNPCSpwnTblData->vSpawn_Loc) == true)
			{
				CNtlPacket packet(sizeof(sGU_OBJECT_CREATE));
				sGU_OBJECT_CREATE * sPacket = (sGU_OBJECT_CREATE *)packet.GetPacketData();

				sPacket->wOpCode = GU_OBJECT_CREATE;
				sPacket->sObjectInfo.objType = OBJTYPE_NPC;
				sPacket->handle = pNPCTblData->tblidx;
				sPacket->sObjectInfo.npcState.sCharStateBase.vCurLoc.x = pNPCSpwnTblData->vSpawn_Loc.x;
				sPacket->sObjectInfo.npcState.sCharStateBase.vCurLoc.y = pNPCSpwnTblData->vSpawn_Loc.y;
				sPacket->sObjectInfo.npcState.sCharStateBase.vCurLoc.z = pNPCSpwnTblData->vSpawn_Loc.z;
				sPacket->sObjectInfo.npcState.sCharStateBase.vCurDir.x = pNPCSpwnTblData->vSpawn_Dir.x;
				sPacket->sObjectInfo.npcState.sCharStateBase.vCurDir.y = pNPCSpwnTblData->vSpawn_Dir.y;
				sPacket->sObjectInfo.npcState.sCharStateBase.vCurDir.z = pNPCSpwnTblData->vSpawn_Dir.z;
				sPacket->sObjectInfo.npcState.sCharStateBase.byStateID = CHARSTATE_SPAWNING;
				sPacket->sObjectInfo.npcBrief.wCurEP = pNPCTblData->wBasic_EP;
				sPacket->sObjectInfo.npcBrief.wCurLP = pNPCTblData->wBasic_LP;
				sPacket->sObjectInfo.npcBrief.wMaxEP = pNPCTblData->wBasic_EP;
				sPacket->sObjectInfo.npcBrief.wMaxLP = pNPCTblData->wBasic_LP;
				sPacket->sObjectInfo.npcBrief.tblidx = pNPCSpwnTblData->mob_Tblidx;

				pSession->InsertIntoMyNpcList(pNPCSpwnTblData->mob_Tblidx, pNPCSpwnTblData->vSpawn_Loc);

				packet.SetPacketLen( sizeof(sGU_OBJECT_CREATE) );
				g_pApp->Send( pSession->GetHandle(), &packet );

			}
		}
	}
	return true;

#pragma endregion Npccheckend
}
void		MobActivity::MonsterRandomWalk(CNtlPacket * pPacket)
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();

	CreatureData * creaturelist;
	for (MONSTERLISTIT it = m_monsterList.begin(); it != m_monsterList.end(); ++it )
	{
		creaturelist = (*it);
		if(creaturelist->IsDead == false)
		{
			if (creaturelist->FightMode == false && (rand() % 30) >= 20)
			{
				CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_STATE));
				sGU_UPDATE_CHAR_STATE * res = (sGU_UPDATE_CHAR_STATE *)packet.GetPacketData();
				res->wOpCode = GU_UPDATE_CHAR_STATE;
				res->handle = creaturelist->MonsterSpawnID;
				res->sCharState.sCharStateBase.bFightMode = false;
				res->sCharState.sCharStateBase.byStateID = CHARSTATE_DESTMOVE;
				res->sCharState.sCharStateDetail.sCharStateDestMove.byDestLocCount = 2;
				res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].x = creaturelist->Spawn_Loc.x + rand() %15;
				res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].y = creaturelist->Spawn_Loc.y;
				res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[0].z = creaturelist->Spawn_Loc.z + rand() %25;
				res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[1].x = creaturelist->Spawn_Loc.x + rand() %15;
				res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[1].y = creaturelist->Spawn_Loc.y;
				res->sCharState.sCharStateDetail.sCharStateDestMove.avDestLoc[1].z = creaturelist->Spawn_Loc.z + rand() %25;

				res->sCharState.sCharStateDetail.sCharStateDestMove.bHaveSecondDestLoc = false;

				packet.SetPacketLen( sizeof(sGU_UPDATE_CHAR_STATE) );
				app->UserBroadcast(&packet);
			}
		}else if(creaturelist->IsDead == true && timeGetTime() >= creaturelist->KilledTime + creaturelist->Spawn_Cool_Time ) 
		{
			printf("spawn time %i cur time %i killed time %i \n", creaturelist->Spawn_Cool_Time, timeGetTime(), creaturelist->KilledTime);
			creaturelist->IsDead = false;
			
			//Spawn monster
			CNtlPacket packet(sizeof(sGU_OBJECT_CREATE));
			sGU_OBJECT_CREATE * res = (sGU_OBJECT_CREATE *)packet.GetPacketData();

			res->wOpCode = GU_OBJECT_CREATE;
			res->sObjectInfo.objType = OBJTYPE_MOB;
			res->handle = creaturelist->MonsterSpawnID;
			res->sObjectInfo.mobState.sCharStateBase.vCurLoc.x = creaturelist->Spawn_Loc.x;
			res->sObjectInfo.mobState.sCharStateBase.vCurLoc.y = creaturelist->Spawn_Loc.y;
			res->sObjectInfo.mobState.sCharStateBase.vCurLoc.z = creaturelist->Spawn_Loc.z;
			res->sObjectInfo.mobState.sCharStateBase.vCurDir.x = creaturelist->Spawn_Dir.x;
			res->sObjectInfo.mobState.sCharStateBase.vCurDir.y = creaturelist->Spawn_Dir.y;
			res->sObjectInfo.mobState.sCharStateBase.vCurDir.z = creaturelist->Spawn_Dir.z;
			res->sObjectInfo.mobState.sCharStateBase.byStateID = CHARSTATE_SPAWNING;
			res->sObjectInfo.mobState.sCharStateBase.bFightMode = false;
			res->sObjectInfo.mobBrief.tblidx = creaturelist->MonsterID;
			res->sObjectInfo.mobBrief.wCurEP = creaturelist->CurEP;
			res->sObjectInfo.mobBrief.wMaxEP = creaturelist->MaxEP;
			res->sObjectInfo.mobBrief.wCurLP = creaturelist->CurLP;
			res->sObjectInfo.mobBrief.wMaxLP = creaturelist->MaxLP;
			res->sObjectInfo.mobBrief.fLastRunningSpeed =  creaturelist->Run_Speed;
			res->sObjectInfo.mobBrief.fLastWalkingSpeed = creaturelist->Walk_Speed;

			packet.SetPacketLen( sizeof(sGU_OBJECT_CREATE) );
			app->UserBroadcast(&packet);

			//Insert into all players list
			app->InsertMonsterIntoAllMyMonsterLists(pPacket, creaturelist->MonsterSpawnID, creaturelist->Spawn_Loc, creaturelist->MonsterID);
		}
	}
}
bool		MobActivity::UpdateDeathStatus(RwUInt32 MobID, bool death_status)
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();

	CreatureData * creaturelist;
	for (MONSTERLISTIT it = m_monsterList.begin(); it != m_monsterList.end(); ++it )
	{
		creaturelist = (*it);
		if(creaturelist != NULL)
		{
			if ( creaturelist->MonsterSpawnID == MobID )
			{
				if(death_status == true)
				{
					creaturelist->isSpawned = false;
					creaturelist->isAggro = false;
					creaturelist->target = 0;
					creaturelist->KilledTime = timeGetTime();
					creaturelist->IsDead = death_status;
					creaturelist->CurLP = creaturelist->MaxLP;
					creaturelist->CurEP = creaturelist->MaxEP;
					//Delete from all Monster lists
					//app->RemoveMonsterFromAllMyMonsterLists(creaturelist->MonsterSpawnID);
				}else{
					creaturelist->IsDead = death_status;
				}
				return true;
			}
		}
	}
	return true;
}

void		MobActivity::SpawnNpcAtLogin(CNtlPacket * pPacket, CClientSession * pSession)
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();

	sVECTOR3 curpos = pSession->plr->GetPosition();
	CSpawnTable* pNPCSpawnTbl = app->g_pTableContainer->GetNpcSpawnTable( pSession->plr->GetWorldID() );
	for ( CTable::TABLEIT itNPCSpawn = pNPCSpawnTbl->Begin(); itNPCSpawn != pNPCSpawnTbl->End(); ++itNPCSpawn )
	{
		sSPAWN_TBLDAT* pNPCSpwnTblData = (sSPAWN_TBLDAT*) itNPCSpawn->second;
		sNPC_TBLDAT* pNPCTblData = (sNPC_TBLDAT*)app->g_pTableContainer->GetNpcTable()->FindData( pNPCSpwnTblData->mob_Tblidx );

		if(app->mob->CreatureRangeCheck(curpos, pNPCSpwnTblData->vSpawn_Loc) == true)
		{

			CNtlPacket packet(sizeof(sGU_OBJECT_CREATE));
			sGU_OBJECT_CREATE * sPacket = (sGU_OBJECT_CREATE *)packet.GetPacketData();
			sPacket->wOpCode = GU_OBJECT_CREATE;
			sPacket->sObjectInfo.objType = OBJTYPE_NPC;
			sPacket->handle = pNPCTblData->tblidx;
			sPacket->sObjectInfo.npcState.sCharStateBase.vCurLoc.x = pNPCSpwnTblData->vSpawn_Loc.x;
			sPacket->sObjectInfo.npcState.sCharStateBase.vCurLoc.y = pNPCSpwnTblData->vSpawn_Loc.y;
			sPacket->sObjectInfo.npcState.sCharStateBase.vCurLoc.z = pNPCSpwnTblData->vSpawn_Loc.z;
			sPacket->sObjectInfo.npcState.sCharStateBase.vCurDir.x = pNPCSpwnTblData->vSpawn_Dir.x;
			sPacket->sObjectInfo.npcState.sCharStateBase.vCurDir.y = pNPCSpwnTblData->vSpawn_Dir.y;
			sPacket->sObjectInfo.npcState.sCharStateBase.vCurDir.z = pNPCSpwnTblData->vSpawn_Dir.z;
			sPacket->sObjectInfo.npcState.sCharStateBase.byStateID = CHARSTATE_SPAWNING;
			sPacket->sObjectInfo.npcBrief.wCurEP = pNPCTblData->wBasic_EP;
			sPacket->sObjectInfo.npcBrief.wCurLP = pNPCTblData->wBasic_LP;
			sPacket->sObjectInfo.npcBrief.wMaxEP = pNPCTblData->wBasic_EP;
			sPacket->sObjectInfo.npcBrief.wMaxLP = pNPCTblData->wBasic_LP;
			sPacket->sObjectInfo.npcBrief.tblidx = pNPCSpwnTblData->mob_Tblidx;

			pSession->InsertIntoMyNpcList(pNPCSpwnTblData->mob_Tblidx, pNPCSpwnTblData->vSpawn_Loc);

			packet.SetPacketLen( sizeof(sGU_OBJECT_CREATE) );
			g_pApp->Send( pSession->GetHandle(), &packet );
				
		}
	}
}
void		MobActivity::SpawnMonsterAtLogin(CNtlPacket * pPacket, CClientSession * pSession)
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();
	sVECTOR3 curpos = pSession->plr->GetPosition();
	CreatureData * creaturelist;

	for (MONSTERLISTIT it = m_monsterList.begin(); it != m_monsterList.end(); ++it )
	{
		creaturelist = (*it);
		if(creaturelist->IsDead == false)
		{
			if(app->mob->CreatureRangeCheck(curpos, creaturelist->Spawn_Loc) == true)
			{
				CNtlPacket packet(sizeof(sGU_OBJECT_CREATE));
				sGU_OBJECT_CREATE * res = (sGU_OBJECT_CREATE *)packet.GetPacketData();

				res->wOpCode = GU_OBJECT_CREATE;
				res->sObjectInfo.objType = OBJTYPE_MOB;
				res->handle = creaturelist->MonsterSpawnID;
				res->sObjectInfo.mobState.sCharStateBase.vCurLoc.x = creaturelist->Spawn_Loc.x;
				res->sObjectInfo.mobState.sCharStateBase.vCurLoc.y = creaturelist->Spawn_Loc.y;
				res->sObjectInfo.mobState.sCharStateBase.vCurLoc.z = creaturelist->Spawn_Loc.z;
				res->sObjectInfo.mobState.sCharStateBase.vCurDir.x = creaturelist->Spawn_Dir.x + rand() % 360;
				res->sObjectInfo.mobState.sCharStateBase.vCurDir.y = creaturelist->Spawn_Dir.y;
				res->sObjectInfo.mobState.sCharStateBase.vCurDir.z = creaturelist->Spawn_Dir.z;
				res->sObjectInfo.mobState.sCharStateBase.byStateID = CHARSTATE_SPAWNING;
				res->sObjectInfo.mobState.sCharStateBase.bFightMode = creaturelist->FightMode;
				res->sObjectInfo.mobBrief.tblidx = creaturelist->MonsterID;
				res->sObjectInfo.mobBrief.wCurEP = creaturelist->CurEP;
				res->sObjectInfo.mobBrief.wMaxEP = creaturelist->MaxEP;
				res->sObjectInfo.mobBrief.wCurLP = creaturelist->CurLP;
				res->sObjectInfo.mobBrief.wMaxLP = creaturelist->MaxLP;
				res->sObjectInfo.mobBrief.fLastRunningSpeed =  creaturelist->Run_Speed;
				res->sObjectInfo.mobBrief.fLastWalkingSpeed = creaturelist->Walk_Speed;
				creaturelist->isAggro = false;
				pSession->InsertIntoMyMonsterList(creaturelist->MonsterSpawnID, creaturelist->Spawn_Loc, creaturelist->MonsterID);

				packet.SetPacketLen( sizeof(sGU_OBJECT_CREATE) );
				g_pApp->Send( pSession->GetHandle(), &packet );
			
			}
		}
	}
}