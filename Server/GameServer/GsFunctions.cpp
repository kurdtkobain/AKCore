#include "stdafx.h"
#include "GameServer.h"

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
void		GsFunctionsClass::SendItemEffect(CClientSession * pSession, TBLIDX effectIdx, TBLIDX itemUseIdx)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();

	CNtlPacket packet3(sizeof(sGU_EFFECT_AFFECTED));
	sGU_EFFECT_AFFECTED * pEffectApply = (sGU_EFFECT_AFFECTED*)packet3.GetPacketData();
	sUSE_ITEM_TBLDAT * itemUseTbl = reinterpret_cast<sUSE_ITEM_TBLDAT*>(app->g_pTableContainer->GetUseItemTable()->FindData(itemUseIdx));
	sSYSTEM_EFFECT_TBLDAT * pEffectTbl = reinterpret_cast<sSYSTEM_EFFECT_TBLDAT*>(app->g_pTableContainer->GetSystemEffectTable()->FindData(effectIdx));
	//Prepare Effect Response
	pEffectApply->effectTblidx = pEffectTbl->tblidx;
	pEffectApply->fActualArgument1 = itemUseTbl->afSystem_Effect_Value[0];
	pEffectApply->fActualArgument2 = itemUseTbl->afSystem_Effect_Value[1];
	pEffectApply->handle = pSession->GetavatarHandle();
	pEffectApply->wOpCode = GU_EFFECT_AFFECTED;
	pEffectApply->wResultCode = GAME_SUCCESS;

	packet3.SetPacketLen(sizeof(sGU_EFFECT_AFFECTED));
	//Validation by Effect Code for better read
	switch (pEffectTbl->effectCode)
	{
		case ACTIVE_DIRECT_HEAL:{
			pSession->plr->pcProfile->wCurLP += (WORD)itemUseTbl->afSystem_Effect_Value[0];
		}
			break;
		case ACTIVE_EP_UP:{
			pSession->plr->pcProfile->wCurEP += (WORD)itemUseTbl->afSystem_Effect_Value[0];
		}
			break;
		case ACTIVE_TELEPORT_BIND:{
			//Need Write
		}
			break;
		case ACTIVE_EP_OVER_TIME:{
			DWORD totalTime = itemUseTbl->dwKeepTimeInMilliSecs;
			while (totalTime != 1000)
			{
				pSession->plr->pcProfile->wCurEP += (WORD)itemUseTbl->afSystem_Effect_Value[0];
				pEffectApply->effectTblidx = pEffectTbl->tblidx;
				pEffectApply->fActualArgument1 = itemUseTbl->afSystem_Effect_Value[0];
				pEffectApply->fActualArgument2 = itemUseTbl->afSystem_Effect_Value[1];
				pEffectApply->handle = pSession->GetavatarHandle();
				pEffectApply->wOpCode = GU_EFFECT_AFFECTED;
				pEffectApply->wResultCode = GAME_SUCCESS;
				g_pApp->Send(pSession->GetHandle(), &packet3);
				app->UserBroadcastothers(&packet3, pSession);
				totalTime -= 1000;
				Sleep(1000);
			}
		}
			break;
		case ACTIVE_RESCUE:{
			//Need Write
		}
			break;
		case ACTIVE_HEAL_OVER_TIME:{
			DWORD totalTime = itemUseTbl->dwKeepTimeInMilliSecs;
			while (totalTime != 1000)
			{
				pSession->plr->pcProfile->wCurLP += (WORD)itemUseTbl->afSystem_Effect_Value[0];
				pEffectApply->effectTblidx = pEffectTbl->tblidx;
				pEffectApply->fActualArgument1 = itemUseTbl->afSystem_Effect_Value[0];
				pEffectApply->fActualArgument2 = itemUseTbl->afSystem_Effect_Value[1];
				pEffectApply->handle = pSession->GetavatarHandle();
				pEffectApply->wOpCode = GU_EFFECT_AFFECTED;
				pEffectApply->wResultCode = GAME_SUCCESS;
				g_pApp->Send(pSession->GetHandle(), &packet3);
				app->UserBroadcastothers(&packet3, pSession);
				totalTime -= 1000;
				Sleep(1000);
			}

		}
			break;
	}
}
//----------------------------------------------------------------------------------//
// NewQuest used only if the db return 0(to see if the char already have this quest
//----------------------------------------------------------------------------------//
void GsFunctionsClass::NewQuest(RwUInt32 CharID, NTL_TS_T_ID tIdQuest, BYTE tsType, DWORD dwParam)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	std::string sql = "INSERT INTO charquestlist (charId,questID,isCompleted,currentStep,type,dwEventData) VALUES(?,?,?,?,?,?)";
	app->db->prepare(sql);
	app->db->setInt(1, CharID);
	app->db->setInt(2, tIdQuest);	
	app->db->setInt(3, 0);
	app->db->setInt(4, 0);
	app->db->setInt(5, tsType);
	app->db->setInt(6, dwParam);
	app->db->execute();
}
//----------------------------------------------------------------------------------//
// QuestStarted Check if the current quest Already started else call NewQuest
//----------------------------------------------------------------------------------//
void GsFunctionsClass::QuestStarted(RwUInt32 CharID, NTL_TS_T_ID tIdQuest, NTL_TS_TC_ID tCurrentStep, NTL_TS_TC_ID tNextAct, BYTE tsType, DWORD dwParam)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	app->db->prepare("SELECT * FROM charquestlist WHERE charId = ? AND questID = ?");
	app->db->setInt(1,CharID);
	app->db->setInt(2, tIdQuest);
	app->db->execute();
	if (app->db->rowsCount() != 0)
	{
		//Is Completed
		if (tNextAct>=100 )
		{
			app->db->prepare("UPDATE charquestlist SET currentStep = ?,nextStep = ?,isCompleted = ? WHERE charId = ? AND questID = ?");
			app->db->setInt(1, tCurrentStep);
			app->db->setInt(2, tNextAct);
			app->db->setInt(3, 1);
			app->db->setInt(4, CharID);
			app->db->setInt(5, tIdQuest);
			app->db->execute();
		}
		else
		{
			app->db->prepare("UPDATE charquestlist SET currentStep = ?,nextStep = ? WHERE charId = ? AND questID = ?");
			app->db->setInt(1, tCurrentStep);			
			app->db->setInt(2, tNextAct);
			app->db->setInt(3, CharID);
			app->db->setInt(4, tIdQuest);
			app->db->execute();
		}
	}		
	else
	{
		NewQuest(CharID, tIdQuest,tsType,dwParam);
	}	
}
//----------------------------------------------------------------------------------//
// Time Quest-GetTmqTblidx By PlayerLevel TODO GET BY MIN AND MAX MEMBER LEVEL?
//----------------------------------------------------------------------------------//
TBLIDX GsFunctionsClass::GetTmq(PlayerInfos *plr)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	CTimeQuestTable* pTmqTable = app->g_pTableContainer->GetTimeQuestTable();
	int iCounter = 1;//0 Is Invalid Tblidx
	while (true)
	{
		iCounter++;
		sTIMEQUEST_TBLDAT* pTmqTblData = reinterpret_cast<sTIMEQUEST_TBLDAT*>(pTmqTable->FindData(iCounter));
		if (pTmqTblData)
		{
				break;
		}		
	}	
	return iCounter;
}
//----------------------------------------------------------------------------------//
// Time Quest-GetNearDifficult By PlayerLevel(Extracted from client code) 
// don't know if need but in all cases here is
//----------------------------------------------------------------------------------//
RwUInt8	GsFunctionsClass::GetTmqLevel(PlayerInfos *plr)
{
	CGameServer * app = (CGameServer*)NtlSfxGetApp();
	TBLIDX idTmq = GsFunctionsClass::GetTmq(plr);
	sTIMEQUEST_TBLDAT* pTIMEQUEST_TBLDAT = reinterpret_cast<sTIMEQUEST_TBLDAT*>(app->g_pTableContainer->GetTimeQuestTable()->FindData(idTmq));
	if (!pTIMEQUEST_TBLDAT)
	{
		printf("Not exist tmq table of index : %i ",idTmq);
		return INVALID_TIMEQUEST_DIFFICULTY;
	}

	sTIMEQUEST_DATASET* pDATASET_EASY = &pTIMEQUEST_TBLDAT->sTimeQuestDataset[TIMEQUEST_DIFFICULTY_EASY];
	sTIMEQUEST_DATASET* pDATASET_NORMAL = &pTIMEQUEST_TBLDAT->sTimeQuestDataset[TIMEQUEST_DIFFICULTY_NORMAL];
	sTIMEQUEST_DATASET* pDATASET_HARD = &pTIMEQUEST_TBLDAT->sTimeQuestDataset[TIMEQUEST_DIFFICULTY_HARD];
	struct sLeastLevel
	{
		RwUInt8 byLeasyLevel;
		RwUInt8 byDifficult;
	};
	std::map<RwUInt8, RwUInt8> mapDifficult;

	mapDifficult[pDATASET_EASY->byMinMemberLevel] = TIMEQUEST_DIFFICULTY_EASY;
	mapDifficult[pDATASET_EASY->byMaxMemberLevel] = TIMEQUEST_DIFFICULTY_EASY;
	mapDifficult[pDATASET_NORMAL->byMinMemberLevel] = TIMEQUEST_DIFFICULTY_NORMAL;
	mapDifficult[pDATASET_NORMAL->byMaxMemberLevel] = TIMEQUEST_DIFFICULTY_NORMAL;
	mapDifficult[pDATASET_HARD->byMinMemberLevel] = TIMEQUEST_DIFFICULTY_HARD;
	mapDifficult[pDATASET_HARD->byMaxMemberLevel] = TIMEQUEST_DIFFICULTY_HARD;

	RwUInt8 byLevelGap;
	sLeastLevel leastDifficult;
	RwUInt8 byAvatarLevel = plr->pcProfile->byLevel;
	std::map<RwUInt8, RwUInt8>::iterator it = mapDifficult.begin();

	leastDifficult.byLeasyLevel = 255;
	leastDifficult.byDifficult = INVALID_TIMEQUEST_DIFFICULTY;

	for (; it != mapDifficult.end(); ++it)
	{
		byLevelGap = (RwUInt8)(abs(it->first - byAvatarLevel));
		if (byLevelGap < leastDifficult.byLeasyLevel)
		{
			leastDifficult.byLeasyLevel = byLevelGap;
			leastDifficult.byDifficult = it->second;
		}
	}

	return leastDifficult.byDifficult;
}