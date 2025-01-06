#ifndef _MODE_TURBO_H
#define _MODE_TURBO_H


#include	"hle_gamemodes.h"
#include    "hle_utility.h"
#include "effects.h"

class CGameModeTurbo : public CGameModeDuel
{
private:

	
public:

	CGameModeLMS();

	virtual void Think();
	void PlayerThink(CBasePlayer*);
	//virtual BOOL CanStartObserver(CBasePlayer *pPlayer);
	virtual void ClientDisconnected(edict_t* pEntity);
	virtual void PlayerSpawn( CBasePlayer *pPlayer );
	virtual void PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor );
	virtual const char *GetGameDescription( void ) { return "HLE Turbo Duel"; } 
	virtual void UpdateGameMode( CBasePlayer *pPlayer );
	//virtual int DeadPlayerWeapons( CBasePlayer *pPlayer );
	//virtual int DeadPlayerAmmo( CBasePlayer *pPlayer );
	//virtual void ResumePlayer(CBasePlayer* pPlayer);
	//virtual void RemovePlayer(CBasePlayer* pPlayer);
	//BOOL FPlayerCanRespawn(CBasePlayer*);

protected:

	virtual void TimeIsUp();
	virtual void StartRoundTime();
	virtual void AnounceOutcome();
	virtual void SpawnValidPlayers();
	virtual void VersesMessage();
	virtual bool MinimumClientsConnected();
	virtual bool IsInRound(edict_t* pEntity);
	virtual void UpdateRound();
	virtual bool IsOver();
};

#endif