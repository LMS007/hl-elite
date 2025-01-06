#ifndef _HLE_BATTLE_H
#define _HLE_BATTLE_H

#include	"hle_gamemodes.h"
#include	"hle_utility.h"


class CGameModeTLMS:  public CHalfLifeTeamplay
{

public:
	

	CGameModeTLMS();
	virtual BOOL CanStartObserver(CBasePlayer *pPlayer);
	virtual BOOL CanStopObserver(CBasePlayer *pPlayer);
	virtual void Think();
	virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
	virtual const char *GetGameDescription( void ) { return "HLE LTS"; }  // this is the game name that gets seen in the server browser
//	virtual void UpdateGameMode( CBasePlayer *pPlayer );  // the client needs to be informed of the current game mode
	virtual BOOL MatchMode() {return FALSE;}

	// lms stuff

	virtual void ResumePlayer(CBasePlayer* pPlayer);
	virtual void RemovePlayer(CBasePlayer* pPlayer);
	virtual int DeadPlayerWeapons( CBasePlayer *pPlayer );
	virtual int DeadPlayerAmmo( CBasePlayer *pPlayer );
	virtual void PlayerSpawn( CBasePlayer *pPlayer );
	virtual void ClientDisconnected(edict_t* pEntity);
	virtual BOOL ClientCommand( CBasePlayer *pPlayer, const char *pcmd );
	virtual void MatchStart(int nType);
	virtual BOOL MatchInProgress(void);
	virtual void GivePlayerBonus(CBasePlayer* pPlayer);
	virtual void StartObserver(CBasePlayer *pPlayer);
//	int GetTeamIndex3( CBasePlayer* pPlayer );

protected:

	void TimeIsUp();
	void StartRoundTime();
	void AnounceOutcome();
	void SpawnValidPlayers();
	void VersesMessage();
	bool MinimumTeamsConnected();
	bool IsInRound(edict_t* pEntity);
	void UpdateRound();
	void ToggleSpawningRestriction();
	bool IsOver();
	void StartRound();
	void EndRound();
	void UpKeep();

	int m_nTeamWins[7];
	int m_nTeamLosses[7];

private:

	int m_nSpawnedPlayers;
	int	m_nLTS;
	hudtextparms_t m_hMessage;
	hudtextparms_t m_hInfo;

	int m_nRoundTime;
	CDelayTime m_dStartRound;

	CDelayTime m_dUpkeep;

	bool m_bInProgress;
	bool m_bAllowRestrictedSpawning;
	bool m_bStartSequence;
	int  m_nNumber;
	int m_nResult;
	int m_nBonus;


	CDelayTime m_dStartMessage;
	CDelayTime m_dEndMessage;



};


#endif