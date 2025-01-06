#ifndef _MODE_DUEL_H
#define _MODE_DUEL_H

#include	"hle_gamemodes.h"
#include    "hle_utility.h"
#include "effects.h"

#define DUEL_BONUS_NONE 0
#define DUEL_BONUS_OWNED 1
#define DUEL_BONUS_PUNISH 4
#define DUEL_BONUS_FLAWLESS 2
#define DUEL_BONUS_IMMORTAL 3
#define DUEL_BONUS_LUCK 5
#define DUEL_BONUS_ANNIAILATION 6
#define DUEL_BONUS_KILLING_SPREE 7
#define DUEL_BONUS_DOMINATION 8
#define ROUND_DELAY 6
#define END_DELAY 2

enum 
{
	RESULT_NONE=0,
	RESULT_TIE,
	RESULT_DRAW,
	RESULT_FORFEIT,
	RESULT_SUICIDE,
	RESULT_DEFEAT,
	RESULT_TIMEUP,
};

class CGameModeRound : public CHalfLifeMultiplay
{

public:

	// round logistics 
	virtual void StartRound()=0;
	virtual void StartRoundTime()=0;
	virtual void EndRound()=0;
	virtual void VersesMessage()=0;
	virtual void AnounceOutcome()=0;

	// spawn control
	virtual void SpawnValidPlayers()=0;
	virtual void ToggleSpawningRestriction()=0;

	// player control
	virtual int  MinimumClients()=0;
	virtual bool MinimumClientsConnected()=0;
	virtual bool IsInRound(edict_t* pEntity)=0;
	//virtual void AddClock(float time);

	// round control
	virtual void UpdateRound()=0;
	virtual bool IsOver()=0;
	virtual void TimeIsUp()=0;
};

class CGameModeDuel : public CGameModeRound
{
public:


    CGameModeDuel();
	BOOL CanStartObserver(CBasePlayer *pPlayer);
	BOOL CanStopObserver(CBasePlayer *pPlayer);
	virtual void SendScoreInfo(CBasePlayer* pSender, CBasePlayer* pReciever=NULL);
	virtual float DeathTime(){return 4;}
	virtual BOOL MatchMode() {return FALSE;}
	virtual void Think();	
	virtual void PlayerThink( CBasePlayer *pPlayer );
    virtual void ClientDisconnected(edict_t* pEntity);
    virtual void PlayerSpawn( CBasePlayer *pPlayer );
	virtual BOOL FPlayerCanRespawn( CBasePlayer *pPlayer );
	float FlPlayerSpawnTime( CBasePlayer *pPlayer );
    virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
    virtual BOOL ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] );   
//	virtual void UpdateGameMode( CBasePlayer *pPlayer );
	int DeadPlayerWeapons( CBasePlayer *pPlayer );
	int DeadPlayerAmmo( CBasePlayer *pPlayer );
	virtual const char *GetGameDescription( void ) { return "HLE Duel"; }  // this is the game name that gets seen in the server browser
	virtual void RemovePlayer(CBasePlayer* pPlayer);
	virtual void ResumePlayer(CBasePlayer* pPlayer);
	//virtual void AddClock(float time);
	virtual void SpawnValidPlayers();
	//virtual void ASDFASDF();
	//virtual void foo();
	
	



protected:

	virtual void ThinkLimit();
	virtual void TimeIsUp();
	virtual void StartRoundTime();
	virtual void AnounceOutcome();
	virtual void StartRound();
	virtual void EndRound();
	virtual void ToggleSpawningRestriction();
	virtual void VersesMessage();
	virtual int  MinimumClients();
	virtual bool MinimumClientsConnected();
	virtual bool IsInRound(edict_t* pEntity);
	virtual void UpdateRound();
	virtual bool IsOver();
	

	int m_nResult;
	int m_nBonus;
	int m_nRoundTime;
	CEntityList m_lEntity;
	bool m_bDeux;
	edict_t* m_pWinner;
	edict_t* m_pLoser;

    bool m_bInProgress;
	bool m_bJustConnected;
	bool m_bAllowRestrictedSpawning;
	bool m_bStartSequence;
	int  m_nNumber;

	CDelayTime m_dStartRound;
	CDelayTime m_dStartMessage;
	CDelayTime m_dEndMessage;	

	hudtextparms_t m_hMessage;
	
	bool LookUpPlayer(edict_t* pEntity);
    void AddPlayer(edict_t* pEntity);
    void DropPlayer(edict_t* pEntity);
	void MovePlayer(edict_t* pEntity);
    void ClearList();
    void RefreshList();
};

#endif