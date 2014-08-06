#include "stdafx.h"
#include "GameServer.h"

bool		CClientSession::CheckMyPlayerAggro(PlayerInfos *mePlr)
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();
	MyMonsterList* mymonsterlist = new MyMonsterList();
	MYMONSTERLIST *mymonsterlistBIS = new MYMONSTERLIST;
	mePlr->myCCSession->FillNewList(mymonsterlistBIS);
	for( MYMONSTERLISTIT it = mymonsterlistBIS->begin(); it != mymonsterlistBIS->end(); it++ )
	{
		mymonsterlist = (*it);
		sVECTOR3 posMo;
		posMo.x = mymonsterlist->Position.x;
		posMo.y = mymonsterlist->Position.y;
		posMo.z = mymonsterlist->Position.z;
		MobActivity::CreatureData* myMob = NULL;
		myMob = app->mob->GetMobByHandle(mymonsterlist->MonsterID);
		if (myMob->isSpawned == true && myMob->IsDead == false && myMob->isAggro == false)
		{
			float distance = app->mob->Distance(posMo, mePlr->GetPosition());
			if (distance <= 20 && distance > 2)
			{
				myMob->target = mePlr->GetAvatarandle();
				myMob->isAggro = true;
				myMob->isAggroByPlayer(plr, app);
				delete mymonsterlistBIS;
				return true;
			}
		}
		else if (myMob->isSpawned == true && myMob->isAggro == true && mePlr->GetAvatarandle() == myMob->target && myMob->IsDead == false)
		{
			float distance = app->mob->Distance(posMo, mePlr->GetPosition());
			if (distance > 20)
			{
				myMob->target = 0;
				myMob->isAggro = false;
				delete mymonsterlistBIS;
				return false;
			}
		}
		else if (myMob->isAggro == true && mePlr->GetAvatarandle() != myMob->target && myMob->IsDead == false);
		else if (myMob->IsDead == false);
		else if (myMob->isSpawned == false);
		else
		{
			printf("i don;t know\n");
			delete mymonsterlistBIS;
			return false;
		}
	}
	return false;
}
bool	GsFunctionsClass::DeleteItemByUIdPlacePos(CNtlPacket * pPacket, CClientSession * pSession, RwUInt32 UniqueID, RwUInt32 Place, RwUInt32 Pos)
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();

	CNtlPacket packet(sizeof(sGU_ITEM_DELETE));
	sGU_ITEM_DELETE * res = (sGU_ITEM_DELETE *)packet.GetPacketData();

	res->wOpCode = GU_ITEM_DELETE;
	res->hSrcItem = UniqueID;
	res->bySrcPlace = Place;
	res->bySrcPos = Pos;

	packet.SetPacketLen(sizeof(sGU_ITEM_DELETE));
	g_pApp->Send( pSession->GetHandle() , &packet );

	return true;
}
/*
 * TODO: Get the right Effect for each thing
 */
 int     GsFunctionsClass::GetBattleResultEffect(int RpEffect)
 {
 	switch (RpEffect)
 	{
 	case DBO_RP_BONUS_TYPE_KNOCKDOWN:
 		return BATTLE_ATTACK_RESULT_KNOCKDOWN;
 	
 	case DBO_RP_BONUS_TYPE_RESULT_PLUS:
 		return BATTLE_ATTACK_RESULT_CRITICAL_HIT;
 
 	case DBO_RP_BONUS_TYPE_EP_MINUS:
 		return BATTLE_ATTACK_TYPE_ENERGY;
 
 	case DBO_RP_BONUS_TYPE_KEEP_TIME_PLUS:
 		return BATTLE_ATTACK_TYPE_ENERGY;
 
 	case DBO_RP_BONUS_TYPE_GUARD_CRASH:
 		return BATTLE_ATTACK_RESULT_CRITICAL_HIT;
 
 	case DBO_RP_BONUS_TYPE_CASTING_TIME_MINUS:
 		return BATTLE_ATTACK_TYPE_ENERGY;
 
 	case DBO_RP_BONUS_TYPE_COOL_TIME_MINUS:
 		return BATTLE_ATTACK_TYPE_ENERGY;
 
 	default:
 		return BATTLE_ATTACK_RESULT_HIT;
 	}
 }
bool	GsFunctionsClass::UpdateCharMoney(CNtlPacket * pPacket, CClientSession * pSession, RwUInt32 ChangeType, RwUInt32 MoneyAmount, RwUInt32 AvatarHandle)
{
	CGameServer * app = (CGameServer*) NtlSfxGetApp();

	CNtlPacket packet(sizeof(sGU_UPDATE_CHAR_ZENNY));
	sGU_UPDATE_CHAR_ZENNY * res = (sGU_UPDATE_CHAR_ZENNY *)packet.GetPacketData();

	res->byChangeType = ChangeType;
	res->dwZenny = MoneyAmount;
	res->handle = AvatarHandle;
	res->wOpCode = GU_UPDATE_CHAR_ZENNY;

	packet.SetPacketLen(sizeof(sGU_UPDATE_CHAR_ZENNY));
	g_pApp->Send( pSession->GetHandle() , &packet );

	return true;
}
DWORD	GsFunctionsClass::CalculePowerLevelPlayer(PlayerInfos *plr)
{
	if (plr)
		return Dbo_CalculatePowerLevel(plr->pcProfile->avatarAttribute.byLastStr, plr->pcProfile->avatarAttribute.byLastCon, plr->pcProfile->avatarAttribute.byLastFoc, plr->pcProfile->avatarAttribute.byLastDex,
			plr->pcProfile->avatarAttribute.byLastSol, plr->pcProfile->avatarAttribute.byLastEng, plr->pcProfile->avatarAttribute.wLastPhysicalOffence, plr->pcProfile->avatarAttribute.wLastPhysicalDefence,
			plr->pcProfile->avatarAttribute.wLastEnergyOffence, plr->pcProfile->avatarAttribute.wLastEnergyDefence, plr->pcProfile->avatarAttribute.wLastAttackRate, plr->pcProfile->avatarAttribute.wLastDodgeRate, plr->pcProfile->avatarAttribute.wLastCurseSuccessRate,
			plr->pcProfile->avatarAttribute.wLastCurseToleranceRate, plr->pcProfile->avatarAttribute.wLastPhysicalCriticalRate, plr->pcProfile->avatarAttribute.wLastEnergyCriticalRate, plr->pcProfile->avatarAttribute.wLastAttackSpeedRate, plr->pcProfile->avatarAttribute.wBaseMaxLP,
			plr->pcProfile->avatarAttribute.wBaseMaxEP, plr->pcProfile->wCurLP, plr->pcProfile->wCurEP, plr->getNumberOfRPBall(), plr->pcProfile->byLevel, 0);
	return 1;
}
DWORD	GsFunctionsClass::CalculePowerLevel(sMOB_TBLDAT* pMOBtData)
{
	if (pMOBtData)
		return Dbo_CalculatePowerLevel(pMOBtData->byStr, pMOBtData->byCon, pMOBtData->byFoc,pMOBtData->byDex, pMOBtData->bySol,
			pMOBtData->byEng, pMOBtData->wBasic_Offence, pMOBtData->wBasic_Physical_Defence, pMOBtData->wBasic_Energy_Defence, pMOBtData->wBasic_Energy_Defence,
			pMOBtData->wAttack_Rate, pMOBtData->wDodge_Rate, pMOBtData->wCurse_Success_Rate, pMOBtData->wCurse_Tolerance_Rate, 5, 
			5, pMOBtData->wAttack_Speed_Rate,pMOBtData->wBasic_LP,pMOBtData->wBasic_EP, pMOBtData->wBasic_LP, pMOBtData->wBasic_EP,1,pMOBtData->byLevel, pMOBtData->byGrade);
	return 1;
}
int     GsFunctionsClass::GetTotalSlotSkill(int charID)
 {
 	CGameServer * app = (CGameServer*)NtlSfxGetApp();
 	app->db->prepare("SELECT * FROM skills WHERE owner_id = ?");
 	app->db->setInt(1, charID);
 	app->db->execute();
 	return app->db->rowsCount();
 }
void	GsFunctionsClass::printError(const char* err)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    WORD saved_attributes;
    /* Save current attributes */
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;
	SetConsoleTextAttribute(hConsole, FOREGROUND_RED);
    printf("%s\n", err);
    /* Restore original attributes */
    SetConsoleTextAttribute(hConsole, saved_attributes);
}

void	GsFunctionsClass::printOk(const char* ok)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    WORD saved_attributes;
    /* Save current attributes */
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;
    SetConsoleTextAttribute(hConsole, FOREGROUND_GREEN);
    printf("%s\n", ok);
    /* Restore original attributes */
    SetConsoleTextAttribute(hConsole, saved_attributes);
}

void	GsFunctionsClass::printDebug(const char* dbg)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    WORD saved_attributes;
    /* Save current attributes */
    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);
    saved_attributes = consoleInfo.wAttributes;
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE);
    printf("%s\n", dbg);
    /* Restore original attributes */
    SetConsoleTextAttribute(hConsole, saved_attributes);
}
void	GsFunctionsClass::DebugSkillType(BYTE skillActType)
{
	switch (skillActType)
	{
	case SKILL_ACTIVE_TYPE_DD:
		printf("SKILL_ACTIVE_TYPE_DD\n");
		break;
	case SKILL_ACTIVE_TYPE_BB:
		printf("SKILL_ACTIVE_TYPE_BB\n");
		break;
	case SKILL_ACTIVE_TYPE_CB:
		printf("SKILL_ACTIVE_TYPE_CB\n");
		break;
	case SKILL_ACTIVE_TYPE_DB:
		printf("SKILL_ACTIVE_TYPE_DB\n");
		break;
	case SKILL_ACTIVE_TYPE_DC:
		printf("SKILL_ACTIVE_TYPE_DC\n");
		break;
	case SKILL_ACTIVE_TYPE_DH:
		printf("SKILL_ACTIVE_TYPE_DH\n");
		break;
	case SKILL_ACTIVE_TYPE_DOT:
		printf("SKILL_ACTIVE_TYPE_DOT\n");
		break;
	}
}