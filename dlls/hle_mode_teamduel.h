#ifndef _MODE_TDUEL_H
#define _MODE_TDUEL_H

#include	"hle_gamemodes.h"
#include    "hle_utility.h"
#include "effects.h"
#include "hle_mode_duel.h"


class CGameModeTeamDuel : public CHalfLifeTeamplay
{
public:


    CGameModeTeamDuel();

	BOOL CanStartObserver(CBasePlayer *pPlayer);
	BOOL CanStopObserver(CBasePlayer *pPlayer);
	virtual void SendScoreInfo(CBasePlayer* pSender, CBasePlayer* pReciever=NULL);
	virtual float DeathTime(){return 4;}
	virtual BOOL MatchMode() {return FALSE;}
	virtual void Think();	
	
    virtual void ClientDisconnected(edict_t* pEntity);
    virtual void PlayerSpawn( CBasePlayer *pPlayer );
	
	
    virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
    
//	virtual void UpdateGameMode( CBasePlayer *pPlayer );
	int DeadPlayerWeapons( CBasePlayer *pPlayer );
	int DeadPlayerAmmo( CBasePlayer *pPlayer );
	virtual const char *GetGameDescription( void ) { return "HLE Team Duel"; }  // this is the game name that gets seen in the server browser
	//virtual void RemovePlayer(CBasePlayer* pPlayer);
	virtual void ResumePlayer(CBasePlayer* pPlayer);

	virtual void SpawnValidPlayers();

	int LookUpTeam(int nTeam);
    void AddTeam(int nTeam);
    void DropTeam(int nTeam);
	void MoveTeam(int nTeam);
	int PlayersOnTeam(int nTeam,bool AreAlive=false);

	
protected:

	
	virtual void TimeIsUp();
	virtual void StartRoundTime();
	virtual void AnounceOutcome();
	virtual void StartRound();
	virtual void EndRound();
	virtual void ToggleSpawningRestriction();
	virtual void VersesMessage();

	virtual int  MinimumTeams();
	virtual bool MinimumTeamsConnected();
	virtual bool IsPlayerInRound(CBasePlayer *pPlayer);
	virtual void UpdateRound();
	virtual bool IsOver();
	

	int m_nResult;
	int m_nBonus;
	int m_nRoundTime;
	CEntityList m_lEntity;
	bool m_bDeux;
	int m_nWinner;
	int m_nLoser;
	
	int m_nTeamList[6];
	int m_nTeamWins[7];
	int m_nTeamLosses[7];

    bool m_bInProgress;
	bool m_bJustConnected;
	bool m_bAllowRestrictedSpawning;
	bool m_bStartSequence;
	int  m_nNumber;

	CDelayTime m_dStartRound;
	CDelayTime m_dStartMessage;
	CDelayTime m_dEndMessage;	

	hudtextparms_t m_hMessage;
	
	
};

#endif