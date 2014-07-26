#include "stdafx.h"
//#include <boost/thread.hpp>
//#include <boost/date_time.hpp>
#include "Vector.h"
#include <list>
#include "Avatar.h"
#include "Character.h"

class CClientSession;

class PlayerInfos
{
public:
	PlayerInfos()
	{
		this->pcProfile = new sPC_PROFILE;
		this->sCharState = new sCHARSTATE;
		this->CurRPBall = 0;
		LastPartyHandle = -1;
	};
	~PlayerInfos(){};
	sPC_PROFILE		*pcProfile;
	sCHARSTATE		*sCharState;
	HSESSION		MySession;
	void		setPlayerStat(sPC_PROFILE* pc, sCHARSTATE *sCharSt)
	{	
		memcpy(this->pcProfile, pc, sizeof(sPC_PROFILE));
		memcpy(this->sCharState, sCharSt, sizeof(sCHARSTATE));
	};
	void		StoreSession(HSESSION ss){this->MySession = ss;};
	void		ClearTheList();
	sVECTOR3	GetPosition(){return this->vCurLoc;};
	sVECTOR3	GetDirection(){return this->vCurDir;};
	void		SetPosition(const sVECTOR3 curPos, const sVECTOR3 curDir)
	{
		this->vCurLoc.x = curPos.x;
		this->vCurLoc.y = curPos.y;
		this->vCurLoc.z = curPos.z;

		this->vCurDir.x = curDir.x;
		this->vCurDir.y = curDir.y;
		this->vCurDir.z = curDir.z;
	};
	int			GetAccountID(){return this->AccountID;};
	void		SetAccountID(const int id){this->AccountID = id;};
	void		SetGuildName(const std::string name){this->guildname = name;};
	std::string		GetGuildName(){return this->guildname;};
	void		SetPlayerName(const std::string name){this->username = name;};
	std::string		GetPlayerName(){return this->username;};
	void		SetWorldID(const int id){this->WorldID = id;};
	int			GetWorldID(){return this->WorldID;};
	void		SetWorldTableID(const int id){this->WorldTableID = id;};
	int			GetWorldTableID(){return this->WorldTableID;};
	void		Setmob_SpawnTime(const RwUInt32 id){this->mob_SpawnTime = id;};
	RwUInt32	Getmob_SpawnTime(){return this->mob_SpawnTime;};
	void		setMyAPP(CGameServer * _app){this->app = _app;};
	void		Setlast_SpawnPos(const sVECTOR3 id){this->last_SpawnPos = id;};
	sVECTOR3	Getlast_SpawnPos(){return this->last_SpawnPos;};
	// PLAYER STAT CALCULE
	void		calculeMyStat(CGameServer * app);
	void		setZero();
	void		UpdateAttribute(RwUInt32 Handle, RwUInt32 Attribute, RwUInt32 Amount);

	void		StoreHandle(const RwUInt32 _avatarHandle){this->avatarHandle = _avatarHandle;};
	RwUInt32	GetAvatarandle(){return this->avatarHandle;};
	int			getNumberOfRPBall(){return this->CurRPBall;};
	void		UpdateRPBall()
	{
		CNtlPacket packet5(sizeof(sGU_UPDATE_CHAR_RP_BALL_MAX));	
 		sGU_UPDATE_CHAR_RP_BALL_MAX * maxBall = (sGU_UPDATE_CHAR_RP_BALL_MAX*)packet5.GetPacketData();
 		CNtlPacket packet6(sizeof(sGU_UPDATE_CHAR_RP_BALL));	
 		sGU_UPDATE_CHAR_RP_BALL * ball = (sGU_UPDATE_CHAR_RP_BALL*)packet6.GetPacketData();
 
 		maxBall->byMaxRPBall = this->getNumberOfRPBall();
 		maxBall->handle = this->GetAvatarandle();
 		maxBall->wOpCode = GU_UPDATE_CHAR_RP_BALL_MAX;
 
 		ball->bDropByTime = true;
 		ball->byCurRPBall = this->getNumberOfRPBall();
 		ball->handle = this->GetAvatarandle();
 		ball->wOpCode = GU_UPDATE_CHAR_RP_BALL;
 
 		packet5.SetPacketLen(sizeof(sGU_UPDATE_CHAR_RP_BALL_MAX));
 		g_pApp->Send(this->MySession, &packet5);
 
 		packet6.SetPacketLen(sizeof(sGU_UPDATE_CHAR_RP_BALL));
 		g_pApp->Send(this->avatarHandle, &packet6);
	};
	void		SetStartRPBall()
	{
		if (this->pcProfile->byLevel >= 5 && this->pcProfile->byLevel <= 9)
			this->CurRPBall = 1;
		else if (this->pcProfile->byLevel >= 15 && this->pcProfile->byLevel <= 24)
			this->CurRPBall = 2;
		else if (this->pcProfile->byLevel >= 25 && this->pcProfile->byLevel <= 29)
			this->CurRPBall = 3;
		else if (this->pcProfile->byLevel >= 30 && this->pcProfile->byLevel <= 34)
			this->CurRPBall = 4;
		else if (this->pcProfile->byLevel >= 35 && this->pcProfile->byLevel <= 39)
			this->CurRPBall = 5;
		else if (this->pcProfile->byLevel >= 40 && this->pcProfile->byLevel <= 44)
			this->CurRPBall = 6;
		else if (this->pcProfile->byLevel >= 45)
			this->CurRPBall = 7;
		this->UpdateRPBall();
	};
	void		SetLevelup(sPC_TBLDAT *Data)
	{
		this->byLevel_Up_Energy_Defence = Data->byLevel_Up_Energy_Defence;
		this->byLevel_Up_Energy_Offence = Data->byLevel_Up_Energy_Offence;
		this->byLevel_Up_EP = Data->byLevel_Up_EP;
		this->byLevel_Up_LP = Data->byLevel_Up_LP;
		this->byLevel_Up_Physical_Defence = Data->byLevel_Up_Physical_Defence;
		this->byLevel_Up_Physical_Offence = Data->byLevel_Up_Physical_Offence;
		this->byLevel_Up_RP = Data->byLevel_Up_RP;
		this->fLevel_Up_Con = Data->fLevel_Up_Con;
		this->fLevel_Up_Dex = Data->fLevel_Up_Dex;
		this->fLevel_Up_Eng = Data->fLevel_Up_Eng;
		this->fLevel_Up_Foc = Data->fLevel_Up_Foc;
		this->fLevel_Up_Sol = Data->fLevel_Up_Sol;
		this->fLevel_Up_Str = Data->fLevel_Up_Str;
	};
	void		LevelUpPlayer()
	{
		this->pcProfile->avatarAttribute.wBasePhysicalOffence += this->byLevel_Up_Physical_Offence;
		this->pcProfile->avatarAttribute.wBasePhysicalDefence += this->byLevel_Up_Physical_Defence;
		this->pcProfile->avatarAttribute.wBaseEnergyOffence += this->byLevel_Up_Energy_Offence;
		this->pcProfile->avatarAttribute.wBaseEnergyDefence += this->byLevel_Up_Energy_Defence;

		this->pcProfile->avatarAttribute.byBaseCon += this->fLevel_Up_Con;
		this->pcProfile->avatarAttribute.byBaseDex += this->fLevel_Up_Dex;
		this->pcProfile->avatarAttribute.byBaseEng += this->fLevel_Up_Eng;
		this->pcProfile->avatarAttribute.byBaseFoc += this->fLevel_Up_Foc;
		this->pcProfile->avatarAttribute.byBaseSol += this->fLevel_Up_Sol;
		this->pcProfile->avatarAttribute.byBaseStr += this->fLevel_Up_Str;

		this->pcProfile->avatarAttribute.wBaseMaxEP += this->byLevel_Up_EP;
		this->pcProfile->avatarAttribute.wBaseMaxRP += this->byLevel_Up_RP;
		this->pcProfile->avatarAttribute.wBaseMaxLP += this->byLevel_Up_LP;

		if (this->pcProfile->byLevel == 5)
			this->CurRPBall = 1;
		if (this->pcProfile->byLevel == 15)
			this->CurRPBall = 2;
		if (this->pcProfile->byLevel == 25)
			this->CurRPBall = 3;
		if (this->pcProfile->byLevel == 30)
			this->CurRPBall = 4;
		if (this->pcProfile->byLevel == 35)
			this->CurRPBall = 5;
		if (this->pcProfile->byLevel == 40)
			this->CurRPBall = 6;
		if (this->pcProfile->byLevel == 45)
			this->CurRPBall = 7;
	}
	void		LastPartyInvited(){};
private:
	MySQLConnWrapper			*db;
public:

void		SaveMe();
void		SavePlayerData();
private:
	sVECTOR3			vCurLoc;
	sVECTOR3			vCurDir;
	int					AccountID;
	int					WorldID;
	int					WorldTableID;
	std::string			guildname;
	std::string			username;
	RwUInt32			mob_SpawnTime;
	sVECTOR3			last_SpawnPos;
	RwUInt32			avatarHandle;
	CGameServer *		app;
	int					CurRPBall;
public: // THIS NEED BE BE PRIVATE IN THE FUTUR
	BYTE			byLevel_Up_LP;
	BYTE			byLevel_Up_EP;
	BYTE			byLevel_Up_RP;
	BYTE			byLevel_Up_Physical_Offence;
	BYTE			byLevel_Up_Physical_Defence;
	BYTE			byLevel_Up_Energy_Offence;
	BYTE			byLevel_Up_Energy_Defence;

	float			fLevel_Up_Str;
	float			fLevel_Up_Con;
	float			fLevel_Up_Foc;
	float			fLevel_Up_Dex;
	float			fLevel_Up_Sol;
	float			fLevel_Up_Eng;
	int					LastPartyHandle;
};