#pragma once

#include "SharedType.h"
#include "MobTable.h"
#include <conio.h>
#include <stdio.h>
#include <dos.h>
#include <ctype.h>
#include <windows.h>

static RwUInt32 m_uiSerialId = 5;
static RwUInt32 m_uiTargetSerialId = 0;
class PlayerInfos;
typedef struct _SBattleData
{
	RwUInt32			uiSerialId;
	RwUInt32			m_uiTargetSerialId;
	RwBool				bAttackMode;
	DWORD				dwCurrTime;
}SBattleData;

typedef struct _SSkillData
{
	RwUInt32			pCharSkillTarget;
	RwUInt32			m_uiSkillTblId;
	RwUInt32			m_bySkillActiveType;
	DWORD				m_uiSkillTime;
}SSkillData;

#define MONSTER_ATTACK_UPDATE_TICK		2000
#define SKILL_TYPE_CASTING				1
#define SKILL_TYPE_NONE					0

static RwUInt32 m_iCurrentHp		=		10000;

#ifndef GS_FUNCTIONS_CLASS_H
#define GS_FUNCTIONS_CLASS_H


class CGameServer;
class CClientSession;
class CNtlPacket;

class GsFunctionsClass
{
public:
	GsFunctionsClass(){};
	~GsFunctionsClass(){};

public:

	bool						DeleteItemByUIdPlacePos(CNtlPacket * pPacket, CClientSession * pSession, RwUInt32 UniqueID, RwUInt32 Place, RwUInt32 Pos);
	bool						UpdateCharMoney(CNtlPacket * pPacket, CClientSession * pSession, RwUInt32 ChangeType, RwUInt32 MoneyAmount, RwUInt32 AvatarHandle);
	DWORD						CalculePowerLevel(sMOB_TBLDAT* pMOBtData);
	DWORD						CalculePowerLevelPlayer(PlayerInfos *plr);
	//System Functions
	void						printError(const char* err);
	void						printOk(const char* err);
	void						printDebug(const char* dbg);
  	//Items
  
  	//Skills Like Debug
  	void						DebugSkillType(BYTE skillActType);
	int							GetTotalSlotSkill(int charID);
};

#endif