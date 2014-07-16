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
	void		StoreHandle(const RwUInt32 _avatarHandle){this->avatarHandle = _avatarHandle;};
	RwUInt32	GetAvatarandle(){return this->avatarHandle;};
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
};